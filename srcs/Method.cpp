#include "webserv.hpp"

int methodToNum(const std::string &name)
{
	if (!name.compare("GET"))
		return GET;
	else if (!name.compare("HEAD"))
		return HEAD;
	else if (!name.compare("PUT"))
		return PUT;
	else if (!name.compare("POST"))
		return POST;
	else if (!name.compare("DELETE"))
		return DELETE;
	else
		return ERROR;
}


std::string numToMethod(const e_method num)
{
	if (num == GET)
		return "GET";
	else if (num == HEAD)
		return "HEAD";
	else if (num == PUT)
		return "PUT";
	else if (num == POST)
		return "POST";
	else if (num == DELETE)
		return "DELETE";
	return "ERROR";
}


std::string makeMethodList(bool *methods)
{
	std::string	list;
	for (int i = 0; i <= POST; i++)
		if (methods[i])
		{
			list.append(numToMethod((e_method)i));
			list.append(", ");
		}
	if (!list.empty())
		list.erase(list.length() - 2);
	return (list);
}
