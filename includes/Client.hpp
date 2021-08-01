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

#define BUF_SIZE 65535
#define CLIENT_DONE_STATUS 1000
#define PARSE_BODY_END 0
#define PARSE_BODY_LEFT 1

#define CGI_NONE 0
#define CGI_PHP 1
#define CGI_CUSTOM 2
class Webserver;

class Client
{
	private:
		uint16_t		_port;
		int				_fd;
		int				_read_fd;
		int				_write_fd;
		e_sock_status	_sock_status;
		HttpRequest		_request;
		HttpResponse	_response;
		std::string		_file_path;
		Config			*_config_location;
		bool			_is_read_finished;

		void makeFilePath();
		void checkFilePath();

	public:
		Client(Socket &socket, int fd);

		std::string makeContentLocation();
		std::string makeAutoindex();
		int isCGIRequest();
		void makeHeadMsg();
		void makeGetMsg(); 
		void makePutMsg();
		void makePostMsg();
		void makeDeleteMsg();
		int				getFd();
		int				getReadFd();
		int				getWriteFd();

		e_sock_status	getSockStatus();
		bool			getIsReadFinished();

		void			setIsReadFinished(bool is_read_finished);
		void			setReadFd(int fd);
		void			setWriteFd(int fd);

		class SocketAcceptException: public std::exception
		{
			virtual const char *what() const throw();
		};
};
