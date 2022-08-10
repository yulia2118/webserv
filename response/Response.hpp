#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "../request/Request.hpp"
#include "../Config/Parser.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include "string.h"
#include "stdlib.h"
#include "wait.h"

#define SSTR( x ) static_cast< std::ostringstream & >( \
		( std::ostringstream() << std::dec << x ) ).str()

void		freeArray(char **ar);
char		**ftSplit(char const *s, char c);

class Response {
	
private:
	std::string							_response;
	std::string							_firstLine;
	std::map<std::string, std::string>	_headers;
	std::string							_bodyContent;
	size_t								_code;
	std::string							_method;
	std::string							_pathToRes;
	std::map<std::size_t, std::string>	_reasonPhrases;
	std::map<std::string, std::string>	_mime;
	std::string							_host;
	int									_port;
	bool								_autoindex;
	std::string							_requestContent;
	std::string							_uploadPath;
	std::string							_nameFile;
	std::string							_res;
	bool								_isCgi;
	std::string							_cgiArg;
	std::string							_methods;
	std::string							_errorPath;

public:
	Response() {}
	Response(Request request);

	std::string		getResponse();
	std::string		createResponse(std::string body);
	std::string		getErrorPage();
	std::string		getMIME();
	std::string		getLastModif();
	std::string		getCurrentDate(void);
	std::string		formatDate(time_t date);
	std::string		getReasonPhrase();
	std::string		getListing();
	std::string		readFile(std::string filepath);
	std::string		getMethod() const;
	void			initReasonPhrases();
	void			initMIME();
	int				getTypeFile(std::string filepath);
	std::string		getMethod();
	std::string		postMethod();
	std::string		deleteMethod();
	std::string		replace(std::string source, std::string to_replace, std::string new_value);
	std::string		runCgi();
};

#endif
