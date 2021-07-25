#include "Client.hpp"

void Client::makeFilePath()
{
	std::string &path = this->_request.getStartLine().path; //считваем путь из запроса
	Config &config = *this->_config_location;

	this->_file_path.clear();
	this->_file_path = config.root; //путь корневого каталога из конфига

	if (!config.location_path.empty())
	{
		std::string dir_path = path.substr(config.location_path.length());
		this->_file_path.append(dir_path); // корень из конфига + часть пути из запроса длины локации из конфига
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