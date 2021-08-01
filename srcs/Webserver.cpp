#include "webserv.hpp"

Webserver::Webserver(const std::string &path, uint32_t max_connection) : _configs(ConfigGroup(path, max_connection)) // инициализация набора серверов с учетом конфига
{
	std::vector<u_int16_t> server_ports;

	for (int i = 0; i < this->_configs.getServerCnt(); i++)
	{
		uint16_t port = this->_configs.getConfig(i).rbegin()->port;
		if (std::find(server_ports.begin(), server_ports.end(), port) == server_ports.end())
			server_ports.push_back(port);
	}

	FT_FD_ZERO(&(this->_fd_read));
	FT_FD_ZERO(&(this->_fd_write));
	for (size_t i = 0; i < server_ports.size(); i++)
	{
		this->_socks.push_back(Socket(server_ports[i], max_connection));
		std::cout << "\033[32m[" << getCurrentTime() << "] : Port " << server_ports[i] << " Opened!\033[0m\n";
		FT_FD_SET(this->_socks[i].getSockFd(),&(this->_fd_read));
	}
}


Webserver::~Webserver() //деструктор
{
	// std::vector<Client>::iterator client_ite = this->_clients.end();
	// for (std::vector<Client>::iterator it = this->_clients.begin(); it != client_ite; it++)
	// 	close(it->getFd());

	// std::vector<Socket>::iterator socket_ite = this->_socks.end();
	// for (std::vector<Socket>::iterator it = this->_socks.begin(); it != socket_ite; it++)
	// 	close(it->getSockFd());
}


void Webserver::startServer() // есть файлы с респонсами и реквестами и файлы с данными(картинка) которые считываются или пишутся
{
	fd_set temp_fd_read, temp_fd_write;
	std::map<int, int> done_info;
	int state;

	while (1)
	{
		memcpy(&temp_fd_read, &(this->_fd_read), sizeof(fd_set)); //копируем из набора файлов те что еще нужны
		memcpy(&temp_fd_write, &(this->_fd_write), sizeof(fd_set));

		state = select(FT_FD_SETSIZE, &temp_fd_read, &temp_fd_write, NULL, NULL); //твой любимый селект
		switch (state)
		{
		case -1:
			throw Webserver::SelectException();
		case 0:
			continue;
		default:
			for (size_t i = 0; i < this->_socks.size(); i++)
				if (FT_FD_ISSET(this->_socks[i].getSockFd(), &(temp_fd_read))) // формируем из слушающих сокетов клиентов с возможностью обмена данными
				{
					int			fd;
					try
					{
						sockaddr	tmp;
						socklen_t	socksize = sizeof(sockaddr_in);

						if ((fd = accept(this->_socks[i].getSockFd(), &tmp, &socksize)) == -1)
							throw Client::SocketAcceptException();
						this->_clients.push_back(Client(this->_socks[i], fd));
						std::cout << "\033[32m[" << getCurrentTime() << "] : Client Connected!\033[0m\n";
					}
					catch(const std::exception& e)
					{
						std::cerr << e.what() << '\n';
						break ;
					}
					FT_FD_SET(fd, &(this->_fd_read));
					FT_FD_SET(fd, &(this->_fd_write));
				}
		//ПОГНАЛИ!!!!!)
			for (unsigned long i = 0; i < this->_clients.size(); i++) //для всех клиентов считываем их запросы из файлов и пишем в логгер
			{
				if (this->_clients[i].getSockStatus() > RECV_BODY)
					continue;
				if (FT_FD_ISSET(this->_clients[i].getFd(), &(temp_fd_read)))
				{
					try
					{
						if (this->readRequest(this->_clients[i]) == CLIENT_END)
							done_info.insert(std::make_pair<int, int>(i, CLIENT_DONE_STATUS));
					}
					catch(int error_status)
					{
						done_info.insert(std::make_pair<int, int>(i, error_status));
					}
				}
			}
			this->handleClientDone(done_info);

			for (unsigned long i = 0; i < this->_clients.size(); i++) //для всех клиентов пишем ответы в файлы и пишем в логгер
			{
				if (this->_clients[i].getSockStatus() <= RECV_BODY)
					continue ;
				if (FT_FD_ISSET(this->_clients[i].getFd(), &(temp_fd_write)))
				{
					try
					{
						this->handleResponse(this->_clients[i]);
						if (this->_clients[i].getSockStatus() == SEND_DONE)
							done_info.insert(std::make_pair<int, int>(i, CLIENT_DONE_STATUS));
					}
					catch (int error_status)
					{
						done_info.insert(std::make_pair<int, int>(i, error_status));
					}
				}
			}
			this->handleClientDone(done_info);
	//здесь логика аля сервлет
			for (unsigned long i = 0; i < this->_clients.size(); i++) // для всех клиентов с которых есть что считать читаем данные и пишем их в респонс и пишем в логгер
			{
				if (this->_clients[i].getReadFd() == -1)
					continue;

				if (!FT_FD_ISSET(this->_clients[i].getReadFd(), &(this->_fd_read)))
					FT_FD_SET(this->_clients[i].getReadFd(), &(this->_fd_read));

				if (!FT_FD_ISSET(this->_clients[i].getReadFd(), &(temp_fd_read)))
					continue;

				try
				{
		//			this->_clients[i].readData(this->_fd_read); здесь нужно считать файл
				}
				catch (int error_status)
				{
					done_info.insert(std::make_pair<int, int>(i, error_status));
				}
			}
			this->handleClientDone(done_info);

			for (unsigned long i = 0; i < this->_clients.size(); i++) // для всех клиентов в которых есть что записать пишем данные из боди реквеста и пишем в логгер
			{
				if (this->_clients[i].getWriteFd() == -1)
					continue;

				if (!FT_FD_ISSET(this->_clients[i].getWriteFd(), &(this->_fd_write)))
					FT_FD_SET(this->_clients[i].getWriteFd(), &(this->_fd_write));

				if (!FT_FD_ISSET(this->_clients[i].getWriteFd(), &(temp_fd_write)))
					continue;

				try
				{
				//	this->_clients[i].writeData(this->_fd_write); здесь нужно записать в файл
				}
				catch (int error_status)
				{
					done_info.insert(std::make_pair<int, int>(i, error_status));
				}
			}
			this->handleClientDone(done_info);
		}
	}
}


int Webserver::readRequest(Client &client) // из файла порсим запрос
{
	char	buff[BUF_SIZE];
	int		len;

	len = read(client.getFd(), buff, BUF_SIZE - 1);
	if (len < 0)
		throw 503;
	if (len == 0)
	{
		client.setIsReadFinished(true);
		return CLIENT_END;
	}
	buff[len] = '\0';
//	client.parseBuffer(buff, len, this->_configs);
	return CLIENT_CONTINUE;
}

void Webserver::handleResponse(Client &client) // парсим ответ в файл во втором случае для гет в первом для остального
{
	(void) client;                            // ВРЕМЕННАЯ ЗАГЛУШКА!!!!!

//	if (client.getSockStatus() == MAKE_READY)
	//	client.makeMsg();
//	else if (client.getSockStatus() == SEND_HEADER || client.getSockStatus() == SEND_BODY)
//	//	client.sendMsg();
}


void Webserver::handleClientDone(std::map<int, int>& done_info) // логгер
{
	std::map<int, int>::reverse_iterator rite = done_info.rend();

	for (std::map<int, int>::reverse_iterator rit = done_info.rbegin(); rit != rite; rit++)
	{
		std::vector<Client>::iterator client = this->_clients.begin() + rit->first;

		if (rit->second != CLIENT_DONE_STATUS)
		{
		//	client->makeBasicHeader();
			std::cout << "\033[33m[ERROR] : " << rit->second << "!!!\033[0m\n";
		//	client->makeErrorStatus(rit->second);
		}
		else
		{
			if (client->getIsReadFinished())
			{
				FT_FD_CLR(client->getFd(), &(this->_fd_read));
				FT_FD_CLR(client->getFd(), &(this->_fd_write));
				close(client->getFd());
				this->_clients.erase(client);
				std::cout << "\033[31m[" << getCurrentTime() << "] : Client Closed!!!\033[0m\n";
			}
		//	else
		//		client->reset(this->_fd_read, this->_fd_write);
		}
	}
	done_info.clear();
}

fd_set &Webserver::getFdRead()
{
	return this->_fd_read;
}

fd_set &Webserver::getFdWrite()
{
	return this->_fd_write;
}

fd_set &Webserver::getFdException()
{
	return this->_fd_exception;
}

const char *Webserver::SelectException::what() const throw()
{
	return "SelectException: fail to run select!";
}

const char *Webserver::SocketReadException::what() const throw()
{
	return "SocketReadException: fail to read socket!";
}
