#include "webserv.hpp"

void ConfigGroup::parseServer(int config_fd, std::string &line, int &gnl_status)
{
	bool is_location_start = false;
	bool is_root_set = false;
	std::vector<Config> server_vector;
	Config server_config;

	gnl_status = ft_getline(config_fd, line);

	while (gnl_status == GNL_OK)
	{
		if (isBlankLine(line))
		{
			gnl_status = ft_getline(config_fd, line);
			continue;
		}
		if (line[0] != '\t')
			break;
		if (line[1] == '\t')
			throw ConfigGroup::ConfigFormatException();

		std::vector<std::string> split = ft_split(line.substr(1), ' ');

        if (!split[0].compare("location"))
		{
            if (split.size() > 2)
                throw ConfigGroup::ConfigFormatException();
			is_location_start = true;
            server_vector.push_back(parseLocation(config_fd, line, split[1], server_config, gnl_status));
			continue;
		}
		if (is_location_start ||
			(split[0].compare("method") && split.size() > 2))
                throw ConfigGroup::ConfigFormatException();

		if (!is_root_set && split[0].compare("root"))
			throw ConfigGroup::ConfigFormatException();

        server_config.parseConfig(split, false);
		is_root_set = true;
		gnl_status = ft_getline(config_fd, line);
    }
	if (server_config.cgi_path.empty() != server_config.cgi_extension.empty())
		throw ConfigGroup::ConfigFormatException();

	if (!checkDupLocation(server_vector))
		throw ConfigGroup::ConfigFormatException();
    server_vector.push_back(server_config);

	_configs.push_back(server_vector);
}

Config ConfigGroup::parseLocation(int config_fd, std::string &line, std::string &loc, Config &server_config, int &gnl_status)
{
	bool is_root_set = false;
    Config location_config(server_config, loc);

	gnl_status = ft_getline(config_fd, line);

	while (gnl_status == GNL_OK)
    {
        if (isBlankLine(line))
		{
			gnl_status = ft_getline(config_fd, line);
            continue;
		}
		
        if (line[0] != '\t' || line[1] != '\t')
			break;
		if (line[2] == '\t')
			throw ConfigGroup::ConfigFormatException();

		std::vector<std::string> split = ft_split(line.substr(2), ' ');

		if (split[0].compare("method") && split.size() > 2)
			throw ConfigGroup::ConfigFormatException();

		if (!is_root_set && split[0].compare("root"))
			throw ConfigGroup::ConfigFormatException();

        location_config.parseConfig(split, true);
		is_root_set = true;
		gnl_status = ft_getline(config_fd, line);
    }
	if (location_config.cgi_path.empty() != location_config.cgi_extension.empty())
		throw ConfigGroup::ConfigFormatException();
    return (location_config);
}

ConfigGroup::ConfigGroup(const std::string &path, uint32_t max_connection = 20): _max_connection(max_connection)
{
	int gnl_status;
	int config_fd = open(path.c_str(), O_RDONLY);

    if (config_fd < 0)
		throw ConfigGroup::NoConfigFileException();

	std::string line;
	gnl_status = ft_getline(config_fd, line);

    while (gnl_status == GNL_OK)
    {
        if (isBlankLine(line))
		{
			gnl_status = ft_getline(config_fd, line);
            continue;
		}
        if (line.compare("server"))
            throw ConfigGroup::ConfigFormatException();
        this->parseServer(config_fd, line, gnl_status);
    }
	close(config_fd);
	if (!checkDupServer())
		throw ConfigGroup::ConfigFormatException();
}

ConfigGroup::~ConfigGroup() {}

/* --------- getter --------- */

int ConfigGroup::getServerCnt()
{
    return (this->_configs.size());
}

uint32_t ConfigGroup::getMaxConnection()
{
    return (this->_max_connection);
}

std::vector<Config> &ConfigGroup::getConfig(int index)
{
    return (this->_configs[index]);
}

bool ConfigGroup::checkDupLocation(std::vector<Config> server_vector)
{
	int size = server_vector.size();

	for (int i = 0; i < size - 1; i++)
	{
		for (int j = i + 1; j < size; j++)
		{
			if (server_vector[i].location_path == server_vector[j].location_path)
				return false;
		}
	}
	return true;
}

bool ConfigGroup::checkDupServer()
{
	int size = getServerCnt();

	for (int i = 0; i < size - 1; i++)
	{
		for (int j = i + 1; j < size; j++)
		{
			if ((_configs[i][0].port == _configs[j][0].port &&
				_configs[i][0].server_name == _configs[j][0].server_name))
					return false;
		}
	}
	return true;
}

const char *ConfigGroup::NoConfigFileException::what() const throw()
{
    return ("NoConfigFileException: there is no file!");
}

const char *ConfigGroup::ConfigFormatException::what() const throw()
{
    return ("ConfigFormatException: Invalid format!");
}
