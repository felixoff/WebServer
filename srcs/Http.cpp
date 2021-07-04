#include "Http.hpp"
#include <iostream>


void Http::insertToHeader(const std::string &key, const std::string &value)
{
	this->_headers.insert(std::make_pair(key, value));
}

std::map<std::string, std::string> &Http::getHeaders()
{
	return this->_headers;
}

std::string Http::getHeaderValue(const std::string &key)
{
	std::map<std::string, std::string>::iterator result = this->_headers.find(key);
	if (result == this->_headers.end())
		return "";
	return (*result).second;
}

std::string &Http::getBody()
{
	return (this->_body);
}

StartLineRes::StartLineRes() : status_code(200) {}


StartLineReq &HttpRequest::getStartLine()
{
	return (this->_start_line);
}

void HttpResponse::sendHeader(int fd)
{
	size_t ret;
	std::string str;

	str += this->_start_line.protocol;
	str += " ";
	str += ft_itos(this->_start_line.status_code);
	str += " ";
	str += getStatusStr(this->_start_line.status_code);
	str += "\r\n";

	std::map<std::string, std::string>::iterator ite = this->_headers.end();
	for (std::map<std::string, std::string>::iterator it = this->_headers.begin(); it != ite; it++)
	{
		str += it->first;
		str += ": ";
		str += it->second;
		str += "\r\n";
	}
	str += "\r\n";
	ret = write(fd, str.c_str(), str.length());
}

int HttpResponse::sendBody(int fd)
{
	size_t	ret;

	ret = write(fd, this->_body.c_str(), this->_body.length());
	if (ret == this->_body.length())
		return SEND_BODY_DONE;
	this->_body.erase(0, ret);
	return SEND_BODY_LEFT;
}


// StartLineRes &HttpResponse::getStartLine()
// {
// 	return (this->_start_line);
// }

// std::ostream &operator<<(std::ostream &o, StartLineReq &rhs)
// {
// 	o << numToMethod(rhs.method) << " " << rhs.path << " " << rhs.protocol << std::endl;
// 	return o;
// }
