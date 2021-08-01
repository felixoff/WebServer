#include "Client.hpp"

#include "Client.hpp"

void Client::makeFilePath()
{
	std::string &path = this->_request.getStartLine().path; //считваем путь из запроса физический
	Config &config = *this->_config_location;

	this->_file_path.clear();
	this->_file_path = config.root; //путь корневого каталога из конфига

	if (!config.location_path.empty())
	{
		std::string dir_path = path.substr(config.location_path.length());
		this->_file_path.append(dir_path); // корень из конфига + часть пути из запроса без локации из конфига
	}
	else
		this->_file_path.append(path);// корень из конфига + путь из запроса

	if (this->_file_path[this->_file_path.length() - 1] == '/')
		this->_file_path.erase(this->_file_path.length() - 1);
}


void Client::checkFilePath() //приверяем валидность пути
{
	Config &config = *this->_config_location;
	std::string index_path;
	struct stat	path_stat;

	if (stat(this->_file_path.c_str(), &path_stat)) // проверка если стат функция выдала ошибку
	{
		switch (errno)
		{
		case EACCES:
		case EFAULT:
		case ENOENT:
		case ENOTDIR:
			throw 404;
		default:
			throw 503;
		}
	}

	if (isFilePath(this->_file_path)) // если по пути находится файл
	{
		std::string last_modified = getHTTPTimeFormat(path_stat.st_mtime);
		this->_response.insertToHeader("Last-modified", last_modified);
		this->_response.insertToHeader("Content-Length", ft_itos(path_stat.st_size));
		return ;
	}

	std::string root = config.root;

	if (config.autoindex) // если включен автоиндекс
		return ;

	index_path = _file_path + "/" + config.index;

	if (stat(index_path.c_str(), &path_stat)) // проверка если стат функция выдала ошибку
	{
		switch (errno)
		{
		case EACCES:
		case EFAULT:
		case ENOENT:
		case ENOTDIR:
			throw 404;
		default:
			throw 503;
		}
	}
	if (isDirPath(index_path))
		throw 404;

	this->_file_path = index_path;
	this->_response.insertToHeader("Content-Length", ft_itos(path_stat.st_size));
}


std::string Client::makeContentLocation() // получение месторасположения
{
	Config &config = *this->_config_location;
	std::string host = this->_request.getHeaderValue("Host");

	std::string content_location = this->_file_path;

	if (content_location.find(config.root) != std::string::npos)
	{
		content_location.erase(0, config.root.length()); // удаляем корень если не корневой конфиг
		if (!config.location_path.empty())
			content_location.insert(0, config.location_path); // вставляем /location-path в начало
	}
	else
		content_location.erase(0, config.server_root.length()); // удаляем корень если корневой конфиг
	return (content_location);
}

std::string Client::makeAutoindex() // создаем листинг каталогов для респонса
{
	DIR *dir;
	struct dirent *curr;
	std::string res;
	std::string url;

	url = this->_request.getHeaderValue("Host") + this->_request.getStartLine().path; // получаем хост и путь
	dir = opendir(this->_file_path.c_str());
	res += "<html>\n<body>\n";
	res += "<h1>Directory listing</h1>\n";
	while ((curr = readdir(dir)) != NULL) // в цикле считываем и записываем все файлы которые содержатся в директории
	{
		if (curr->d_name[0] != '.')
		{
			res += "<a href=\"http://" + url;
			res += curr->d_name;
			res += "\">";
			res += curr->d_name;
			res += "</a><br>\n";
		}
	}
	closedir(dir);
	res += "</body>\n</html>\n";
	return (res);
}

int Client::isCGIRequest() // проверка что у нас cgi запро
{
	std::vector<std::string> split = ft_split(this->_request.getStartLine().path, '.'); // разделяем реквест с целью найти что после точки

	if (!this->_config_location->cgi_extension.empty()) // установка кастомного cgi если после точки расширение кастомное
	{
		if (!split.rbegin()->compare(this->_config_location->cgi_extension))
			return CGI_CUSTOM;
	}
	if (!this->_config_location->php_path.empty()) // установка php cgi если после точки расширение php
	{
		if (!split.rbegin()->compare("php"))
			return CGI_PHP;
	}
	return CGI_NONE;
}

void Client::makeHeadMsg() // создаем респонс для head запроса
{
	this->checkFilePath();
	std::string content_location = this->makeContentLocation();

	this->_response.insertToHeader("Content-Location", content_location);
	this->_response.insertToHeader("Content-Language", "ko");
	this->_response.insertToHeader("Content-Type", "text/plain");
	this->_sock_status = SEND_HEADER;
}

void Client::makeGetMsg() // создаем ответ для гет запроса
{
	struct stat info;
	std::string line;

	// if (this->isCGIRequest())
	// {
	// 	this->execCGI();
	// 	return ;
	// }

	this->makeHeadMsg();
	this->_sock_status = MAKE_MSG;

	stat(this->_file_path.c_str(), &info); // проверяем валидность файла по пути и если не выкинет ошибку идем дальше

	if (isDirPath(_file_path) && this->_config_location->autoindex == true)
	{
		line = this->makeAutoindex();
		this->_response.getBody() += line;
	}

	if (this->_config_location->autoindex == false)
	{
		this->_read_fd = open(this->_file_path.c_str(), O_RDONLY);
		if (this->_read_fd == -1)
			throw 503;
	}
}


void Client::makePutMsg() // парсим ответ для пут запроса
{
	int		fd;

	if (isFilePath(this->_file_path))
	{
		fd = open(_file_path.c_str(), O_WRONLY | O_TRUNC, 0666);
		if (fd == -1)
			throw 403;
		this->_response.getStartLine().status_code = 204;
	}
	else
	{
		if (isDirPath(this->_file_path))
			throw 400;
		fd = open(_file_path.c_str(), O_CREAT | O_WRONLY, 0666);
		if (fd == -1)
			throw 503;
		this->_response.getStartLine().status_code = 201;
		this->_response.insertToHeader("Content-Length", "0");
	}
	this->_write_fd = fd;
	std::string content_location = this->makeContentLocation();

	this->_response.insertToHeader("Location", content_location);
	this->_response.insertToHeader("Content-Location", content_location);
}

void Client::makePostMsg() // парсим ответ для пост запроса
{
	if (_response.getBody().size() == 0)
		makeGetMsg();
	// else if (this->isCGIRequest())
	// 	this->execCGI();
}

void Client::makeDeleteMsg() // парсим ответ для делет запроса
{
	if (isDirPath(this->_file_path))
		throw 400;

	if (!isFilePath(this->_file_path))
	{
		switch (errno)
		{
		case EACCES:
			throw 403;

		case ENOENT:
			throw 404;

		default:
			throw 400;
		}
	}
	this->_response.getStartLine().status_code = 204;
	unlink(this->_file_path.c_str());
	this->_sock_status = SEND_HEADER;
}


Client::Client(Socket &socket, int fd):
	_port(socket.getPort()),
	_fd(fd),
	_read_fd(-1),
	_write_fd(-1),
	_sock_status(INITIALIZE),
	_config_location(NULL),
	_is_read_finished(false)
{
}

int Client::getFd()
{
	return this->_fd;
}

int Client::getReadFd()
{
	return this->_read_fd;
}

int Client::getWriteFd()
{
	return this->_write_fd;
}

e_sock_status Client::getSockStatus()
{
	return this->_sock_status;
}

bool Client::getIsReadFinished()
{
	return this->_is_read_finished;
}

void Client::setIsReadFinished(bool is_read_finished)
{
	this->_is_read_finished = is_read_finished;
}

void Client::setReadFd(int fd)
{
	this->_read_fd = fd;
}

void Client::setWriteFd(int fd)
{
	this->_write_fd = fd;
}

const char *Client::SocketAcceptException::what() const throw()
{
	return ("SocketAcceptException: accept error!");
}