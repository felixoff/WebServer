#include "webserv.hpp"

Webserver::Webserver(const std::string &path, uint32_t max_connection) : _configs(ConfigGroup(path, max_connection))
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


Webserver::~Webserver()
{
	std::vector<Client>::iterator client_ite = this->_clients.end();
	for (std::vector<Client>::iterator it = this->_clients.begin(); it != client_ite; it++)
		close(it->getFd());

	std::vector<Socket>::iterator socket_ite = this->_socks.end();
	for (std::vector<Socket>::iterator it = this->_socks.begin(); it != socket_ite; it++)
		close(it->getSockFd());
}

void Webserver::startServer()
{
	fd_set temp_fd_read, temp_fd_write;
	std::map<int, int> done_info;
	int state;

	while (1)
	{
		memcpy(&temp_fd_read, &(this->_fd_read), sizeof(fd_set));
		memcpy(&temp_fd_write, &(this->_fd_write), sizeof(fd_set));

		state = select(FT_FD_SETSIZE, &temp_fd_read, &temp_fd_write, NULL, NULL);
		switch (state)
		{
		case -1:
			throw Webserver::SelectException();
		case 0:
			continue;
		default:
			for (size_t i = 0; i < this->_socks.size(); i++)
				if (FT_FD_ISSET(this->_socks[i].getSockFd(), &(temp_fd_read)))
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

			for (unsigned long i = 0; i < this->_clients.size(); i++)
			{
				//Чтение данных
			}
			this->handleClientDone(done_info);

			for (unsigned long i = 0; i < this->_clients.size(); i++)
			{
				//Запись данных
			}
			this->handleClientDone(done_info);//Запись в логгер
		 }
	}
}

void Webserver::handleClientDone(std::map<int, int>& done_info)
{
//LOGGER
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
