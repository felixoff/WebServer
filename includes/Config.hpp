#pragma once

#ifdef __linux
	#include <stdint.h>
#endif
#include <string>
#include <vector>

struct Config
{
	std::string		server_name;
	std::string		root;
	uint16_t		port;				// по умолчанию = = 80;
	std::string		index;				// по умолчанию = = index.html
	std::string		error_page;			// по умолчанию = = error.html
	uint64_t		body_length;		// по умолчанию = = 1M
	bool			autoindex;			// по умолчанию = = off
	uint32_t		timeout;			// по умолчанию = = 5s
	std::string		auth;
	bool			method[5];			// по умолчанию = = GET HEAD
	std::string		cgi_path;
	std::string		cgi_extension;
	std::string		php_path;
	std::string		server_root;
	std::string		location_path;

	Config();
	Config(const Config &, std::string &);

	void parseConfig(std::vector<std::string> &split, bool is_location);
};
