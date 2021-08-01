#include "webserv.hpp"


Config::Config(): // конструируем конфигурационный файл
	port(80),
	body_length(1000000),
	autoindex(false),
	timeout(5)
{
	method[0] = method[1] = true;
	method[2] = method[3] = method[4] = false;
}


Config::Config(const Config &src, std::string &loc_path): // конструируем конфигурационный файл
	server_name(src.server_name),
	root(src.root),
	port(src.port),
	index(src.index),
	error_page(src.error_page),
	body_length(src.body_length),
	autoindex(src.autoindex),
	timeout(src.timeout),
	auth(src.auth),
	cgi_path(src.cgi_path),
	cgi_extension(src.cgi_extension),
	php_path(),
	server_root(src.server_root),
	location_path(loc_path)
{
	for (int i = 0; i < 5; i++)
		method[i] = src.method[i];
}


void Config::parseConfig(std::vector<std::string> &split, bool is_location) // парсим конфигурационный файл (одну строку и так построчно циклическим вызовом из group)
{
	if (split.size() < 2)
		throw ConfigGroup::ConfigFormatException();
	if (!split[0].compare("server_name"))
	{
		if (is_location)
			throw ConfigGroup::ConfigFormatException();
		this->server_name = split[1];
	}
	else if (!split[0].compare("port"))
	{
		if (is_location)
			throw ConfigGroup::ConfigFormatException();
		this->port = ft_atoi(split[1]);
		if (this->port < 0 || this->port > 65535)
			throw ConfigGroup::ConfigFormatException();
	}
	else if (!split[0].compare("index"))
	{
		this->index = split[1];
	}
	else if (!split[0].compare("error_page"))
	{
		this->error_page = split[1];
	}
	else if (!split[0].compare("body_length"))
	{
		this->body_length = ft_atoi(split[1]);
		if (this->body_length <= 0)
			throw ConfigGroup::ConfigFormatException();
	}
	else if (!split[0].compare("autoindex"))
	{
		if (!split[1].compare("on"))
			this->autoindex = true;
		else if (!split[1].compare("off"))
			this->autoindex = false;
		else
			throw ConfigGroup::ConfigFormatException();
	}
	else if (!split[0].compare("timeout"))
	{
		this->timeout = ft_atoi(split[1]);
		if (this->timeout <= 0)
			throw ConfigGroup::ConfigFormatException();
	}
	else if (!split[0].compare("auth"))
	{
		this->auth = split[1];
	}
	else if (!split[0].compare("method"))
	{
		for (int i = 0; i < 5; i++)
			this->method[i] = false;
		for (size_t i = 1; i < split.size(); i++)
		{
			if (!split[i].compare("GET"))
				this->method[GET] = true;
			else if (!split[i].compare("HEAD"))
				this->method[HEAD] = true;
			else if (!split[i].compare("PUT"))
				this->method[PUT] = true;
			else if (!split[i].compare("POST"))
				this->method[POST] = true;
			else if (!split[i].compare("DELETE"))
				this->method[DELETE] = true;
			else
				throw ConfigGroup::ConfigFormatException();
		}
	}
	else if (!split[0].compare("root"))
	{
		if (is_location)
		{
			this->root = this->server_root;
			this->root.append("/");
			this->root.append(split[1]);
		}
		else
		{
			this->root = split[1];
			this->server_root = this->root;
			// this->index = this->root + "/index.html";
			// this->error_page = this->root + "/error.html";
		}
	}
	else if (!split[0].compare("cgi_path"))
	{
		this->cgi_path = split[1];
	}
	else if (!split[0].compare("cgi_extension"))
	{
		this->cgi_extension = split[1];
	}
	else if (!split[0].compare("php_path"))
	{
		this->php_path = split[1];
	}
	else
		throw ConfigGroup::ConfigFormatException();
}
