#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "../Config/Parser.hpp"

class Request {

private:
	int							checkValid(Config *server);
	int							checkValidPath(Config *server);
	int							dirExists(const char* const path);
	void						removeDuplicateSlash();
	std::string					removeDelim(std::string item, std::string const &set);
	std::string					_allowMethods;
	std::vector<std::string>	splitLine(std::string buffer);
	std::vector<std::string>	splitFirst(std::string buffer);
	std::vector<std::string>	splitHeaders(std::string buffer);

public:
	Request() {};
	Request(char *buffer, Config *server);
	
	int						statusCode;
	bool					autoindex;
	std::string				method;
	std::string				reqPath;
	std::string				path;
	std::string				version;
	std::string				body;
	std::string				cgiArg;
	bool					isCgi;
	int						host;
	int						port;
	std::string				hostString;
	std::string				pathPost;
	std::string				errorPath;
	std::map<std::string,
			std::string>	headers;

	std::string				getAllowMethods();

};

#endif
