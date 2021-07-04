#pragma once

#include "Http.hpp"
#include "Config.hpp"
#include "utils.hpp"
#include "Socket.hpp"
#include <sys/socket.h>
 #include <sys/stat.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <string>

enum e_sock_status
{
	INITIALIZE,
	RECV_START_LINE,
	RECV_HEADER,
	RECV_BODY,
	MAKE_READY,
	PROC_CGI_HEADER,
	PROC_CGI_BODY,
	MAKE_MSG,
	SEND_HEADER,
	SEND_BODY,
	SEND_DONE
};


class Webserver;

class Client
{
	private:
		HttpRequest		_request;
		HttpResponse	_response;
				uint16_t	_port;
				int  _fd;
//		std::string		_file_path;
	//	    Config			*_config_location;
	//	Socket			*_socket;


	public:
		Client(Socket &socket, int fd);

		int				getFd();

		class SocketAcceptException: public std::exception
		{
			virtual const char *what() const throw();
		};
};
