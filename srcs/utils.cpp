#include "webserv.hpp"


int ft_getline(int fd, std::string &line) // аля гнл на плюсах
{
	size_t len = 0;
	char buff[GNL_BUFFER];
	static std::string str[FT_FD_SETSIZE];

	if (fd < 0 || fd > FT_FD_SETSIZE || read(fd, buff, 0))
		throw GNLException();
	while (str[fd].find('\n') == std::string::npos && ((len = read(fd, buff, GNL_BUFFER - 1)) > 0))
	{
		buff[len] = '\0';
		str[fd].append(buff);
	}
	if (len < 0)
	{
		str[fd].erase();
		throw GNLException();
	}
	if (len == 0 && str[fd].empty()) // когда файл считан
	{
		str[fd].erase();
		line = "";
		return GNL_EOF;
	}

	len = str[fd].find('\n');
	len = (len == std::string::npos ? str[fd].length() : len);
	line = str[fd].substr(0, len);
	int result = (len == str[fd].length() ? GNL_EOF : GNL_OK);
	if (result == GNL_EOF)
		str[fd].erase();
	else
		str[fd].erase(0, len + 1);
	return result;
}

const char *GNLException::what() const throw()
{
    return ("GNLException: Error while running ft_getline!");
}


std::vector<std::string> ft_split(std::string str, char delim)//сплит в вектор
{
	std::vector<std::string> answer;
	size_t curr = 0;
	int prev = 0;
	answer.clear();

	curr = str.find(delim);
	while (curr != std::string::npos)
	{
		answer.push_back(str.substr(prev, curr - prev));
		prev = curr + 1;
		curr = str.find(delim, prev);
	}
	answer.push_back(str.substr(prev, curr - prev));

	return answer;
}


bool isBlankLine(const std::string &line) // проверка пустая ли строка
{
	for (size_t i = 0; i < line.size(); i++)
	{
        if (line[i] != ' ' && line[i] != '\t')
            return false;
    }
    return true;
}


void ft_trim(std::string &str, const std::string cut) // отрезает с обоих сторон
{
	std::size_t found = str.find_last_not_of(cut);
	if (found != std::string::npos)
		str.erase(found + 1);
	else
		str.clear();

	found = str.find_first_not_of(cut);
	if (found != std::string::npos)
		str.erase(0, found);
	else
		str.clear();
}

int	ft_atoi(const std::string &str) //атой на плюсах
{
	long long result = 0;
	int minus_flag = 1, index = 0;

	if (!str.empty() && (str[index] == '+' || str[index] == '-') && static_cast<int>(str.length()) > index)
	{
		if (str[index] == '-')
			minus_flag = -1;
		index++;
	}
	while ('0' <= str[index] && str[index] <= '9' && static_cast<int>(str.length()) > index)
	{
		result *= 10;
		result += (str[index] - '0');
		index++;
	}
	result *= minus_flag;
	return static_cast<int>(result);
}

std::string ft_itos(int num) // число в строку
{
	std::string str;
	long long	nbr = (long long)num;

	if (num == 0)
		return ("0");
	if (num < 0)
		nbr *= -1;
	while (nbr != 0)
	{
		str = static_cast<char>((nbr % 10) + 48) + str;
		nbr /= 10;
	}
	if (num < 0)
		str = "-" + str;
	return (str);
}


// std::string ft_ultohex(unsigned long num) // преобразование лонг в 16 строку
// {
// 	std::string str;
// 	unsigned long tmp;

// 	if (num == 0)
// 		return ("0");
// 	while (num != 0)
// 	{
// 		tmp = num % 16;
// 		tmp = (tmp >= 10 ? (tmp + 'A' - 10) : (tmp + '0'));
// 		str = static_cast<char>(tmp) + str;
// 		num /= 16;
// 	}
// 	return (str);
// }


// unsigned long	ft_uhextol(const std::string &str) // преобразование 16 строки в лонг
// {
// 	unsigned long	result = 0;
// 	int				index = 0;

// 	while ((('0' <= str[index] && str[index] <= '9') ||
// 			('A' <= str[index] && str[index] <= 'F') ||
// 			('a' <= str[index] && str[index] <= 'f')) && static_cast<int>(str.length()) > index )
// 	{
// 		result *= 16;
// 		if (str[index] <= '9')
// 			result += (str[index] - '0');
// 		else if (str[index] <= 'F')
// 			result += (str[index] - 'A' + 10);
// 		else
// 			result += (str[index] - 'a' + 10);
// 		index++;
// 	}
// 	return result;
// }


std::string getHTTPTimeFormat(time_t time) // получение времени в нужном формате
{
	char s[150];
	struct tm *tm_time = std::gmtime(&time);

	strftime(s, sizeof(s), "%a, %d %b %Y %T GMT", tm_time);
    return (s);
}


std::string getCurrentTime() //получение текущего времени
{
	struct timeval time;
	gettimeofday(&time, NULL);
	return getHTTPTimeFormat(time.tv_sec);
}

bool isFilePath(const std::string &path) // проверка явлеется ли по пути файл с помощью stat
{
	struct stat info;

	if (stat(path.c_str(), &info) == 0)
	{
		if (S_ISREG(info.st_mode))
			return 1;
		else
			return 0;
	}
	else
		return 0;
}


bool isDirPath(const std::string &path) // проверка явлеется ли по пути директория с помощью stat
{
	struct stat info;

	if (stat(path.c_str(), &info) == 0)
	{
		if (S_ISDIR(info.st_mode))
			return 1;
		else
			return 0;
	}
	else
		return 0;
}

std::string getStatusStr(uint16_t code) // конвертор кода ответа в строку
{
    switch (code) {
        case 200:
            return ("OK");
        case 201:
            return ("Created");
        case 202:
            return ("Accepted");
		case 204:
			return ("No Content");
        case 400:
            return ("Bad Request");
        case 401:
			return ("Unauthorized");
		case 403:
            return ("Forbidden");
        case 404:
            return ("Not Found");
		case 405:
			return ("Method Not Allowed");
		case 413:
			return ("Payload Too Large");
		case 418:
			return ("I'm a teapot");
        case 500:
            return ("Internal Server Error");
		case 503:
			return ("Service Unavailable");
		case 505:
			return ("HTTP Version Not Supported");
    }
    return ("");
}

std::string ft_inet_ntoa(unsigned int addr) // ip адрес в строку
{
	unsigned int n = addr;

	std::string res = std::to_string(n % 256) + ".";
	res += std::to_string((n / 256) % 256) + ".";
	res += std::to_string((n / 256 / 256) % 256) + ".";
	res += std::to_string(n / 256 / 256 / 256);

	return (res);
}


uint16_t ft_htons(uint16_t port) //шорт узловой в сетевой порядок расположения байтов
{
	uint16_t res = (((((unsigned short)(port) & 0xFF)) << 8) | (((unsigned short)(port) & 0xFF00) >> 8));

	return (res);
}


u_int32_t ft_htonl(unsigned long int addr)//лонг узловой в сетевой порядок расположения байтов
{
	u_int32_t res = (((((unsigned long)(addr) & 0xFF)) << 24) | \
					((((unsigned long)(addr) & 0xFF00)) << 8) | \
					((((unsigned long)(addr) & 0xFF0000)) >> 8) | \
					((((unsigned long)(addr) & 0xFF000000)) >> 24));

	return (res);
}
