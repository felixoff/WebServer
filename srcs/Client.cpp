#include "Client.hpp"


Client::Client(Socket &socket, int fd):
	_port(socket.getPort()),
	_fd(fd)
	//_socket(&socket)
{}

int Client::getFd()
{
	return this->_fd;
}

const char *Client::SocketAcceptException::what() const throw()
{
	return ("SocketAcceptException: accept error!");
}
