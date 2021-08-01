#include "Http.hpp"
#include <iostream>


void Http::insertToHeader(const std::string &key, const std::string &value) // вставляем строку в хедер
{
	this->_headers.insert(std::make_pair(key, value));
}

std::map<std::string, std::string> &Http::getHeaders() // получаем карту хедеров
{
	return this->_headers;
}


std::string Http::getHeaderValue(const std::string &key) // получаем значение хедера по ключу
{
	std::map<std::string, std::string>::iterator result = this->_headers.find(key);
	if (result == this->_headers.end())
		return "";
	return (*result).second;
}

std::string &Http::getBody()// получаем тело хедера
{
	return (this->_body);
}

StartLineRes::StartLineRes() : status_code(200) {}  //конструктор по умолчанию с кодом успеха


StartLineReq &HttpRequest::getStartLine() //геттер
{
	return (this->_start_line);
}

void HttpResponse::sendHeader(int fd) 
{
	size_t ret;
	std::string str;

	str += this->_start_line.protocol; //передаем протокол код и
	str += " ";
	str += ft_itos(this->_start_line.status_code);
	str += " ";
	str += getStatusStr(this->_start_line.status_code);
	str += "\r\n";

	std::map<std::string, std::string>::iterator ite = this->_headers.end();
	for (std::map<std::string, std::string>::iterator it = this->_headers.begin(); it != ite; it++) // в цикле записываем в строку остальные хедеры ответа
	{
		str += it->first;
		str += ": ";
		str += it->second;
		str += "\r\n";
	}
	str += "\r\n";
	ret = write(fd, str.c_str(), str.length()); // запись строкового ответа в файл
}

int HttpResponse::sendBody(int fd) 
{
	size_t	ret;

	ret = write(fd, this->_body.c_str(), this->_body.length());
	if (ret == this->_body.length())
		return SEND_BODY_DONE; // флаг окончания передачи тела
	this->_body.erase(0, ret);
	return SEND_BODY_LEFT; // флаг если тело передано не полностью
}


StartLineRes &HttpResponse::getStartLine() //геттер
{
	return (this->_start_line);
}

std::ostream &operator<<(std::ostream &o, StartLineReq &rhs) //  аля toString
{
	o << numToMethod(rhs.method) << " " << rhs.path << " " << rhs.protocol << std::endl;
	return o;
}
