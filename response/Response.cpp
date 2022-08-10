#include "Response.hpp"

Response::Response(Request request)
{
	_code = request.statusCode;
	_res = request.reqPath;
	_method = request.method;
	
	_pathToRes = request.path;
	_requestContent = request.body;
	if (getTypeFile(_pathToRes) == 0)
		_uploadPath = _pathToRes + "/POST.txt";
	else
		_uploadPath = "";
	_host = request.hostString;
	_port = request.port;
	_autoindex = request.autoindex;

	_errorPath = request.errorPath;

	_isCgi = request.isCgi;
	_cgiArg = request.cgiArg;
	if ((_methods = request.getAllowMethods()) == "")
		_methods = "GET";
	initReasonPhrases();
	initMIME();
}

std::string Response::getResponse() {
	if ((_code == 413) || (_code == 404) || (_code == 505))
		return (createResponse(getErrorPage()));
	if (_code == 405) {
		char **methods_c = ftSplit(_methods.c_str(), ' ');
		std::vector<std::string> methods;
		std::string methods_str;
		int i = 0;
		while (methods_c[i] != NULL)
		{
			methods.push_back(std::string(methods_c[i]));
			++i;
		}
		freeArray(methods_c);
		for (size_t i = 0; i < methods.size(); ++i)
		{
			methods_str += methods[i];
			if (i < methods.size() - 1)
				methods_str += ", ";
		}
		_headers["Allow"] = methods_str;
		return (createResponse(getErrorPage()));
	}

	if (_code == 301) {
		_headers["Location"] = _pathToRes;
		return(createResponse(""));
	}
	if ((getTypeFile(_pathToRes) == 0) && _autoindex && _method == "GET")
		return (createResponse(getListing()));
	if (_isCgi)
		return runCgi();

	if (_method == "GET")
		return getMethod();
	if (_method == "POST")
		return postMethod();
	if (_method == "DELETE")
		return deleteMethod();
	return NULL;
}

std::string Response::runCgi() {
	int		pid;
	int		status;
	pid = fork();
	if (pid == 0)
	{
		std::string met =  "REQUEST_METHOD=" + _method;
		std::string path =  "PATH_INFO=" + _pathToRes;
		char *argv[2];
		char *envp[4];
		argv[0] = (char *)_pathToRes.c_str();
		argv[1] = (char *)NULL;
		envp[0] = (char *)(met.c_str());
		envp[1] = (char *)("SERVER_PROTOCOL=HTTP/1.1");
		envp[2] = (char *)(path.c_str());
		envp[3] = (char *)NULL;
		if (!freopen("./cgi_in.html", "w", stdout))
			std::cout << "NO OPEN OUT\n";
		std::cout << _cgiArg;
		if (!freopen("./cgi_in.html", "r", stdin))
			std::cout << "NO OPEN IN\n";
		if (!freopen("./cgi_out.html", "w", stdout))
			std::cout << "NO OPEN OUT\n";
		if (execve(_pathToRes.c_str(), argv, envp) == -1)
		{
			std::cout << "CGI ERROR\n";
			exit(-1);
		}
		exit(0);
	}
	wait(&status);
	if (WIFEXITED(status) != 0)
		status = WEXITSTATUS(status);
	if (status != 0)
		_code = 501;
	else
		_code = 200;
	std::stringstream answer;
	std::ifstream out("./cgi_out.html");
	if (out.is_open())
	{
		answer << "<body><h3><span style=\"color: #5ba865; font-size: 1em\">"
				<< out.rdbuf() << "</span></h3></body>";
		out.close();
		remove("./cgi_out.html");
		remove("./cgi_in.html");
	}
	return (createResponse(answer.str()));
}

std::string Response::getListing() {
	std::string base;
	std::string listing;
	std::string link_base;
	struct dirent *en;
	DIR *dr;

	base = readFile("./html/www/listing.html");
	base = replace(base, "$1", _res);
	if ((dr = opendir(_pathToRes.c_str())) == NULL)
	{
		_code = 500;
		return ("ERROR OPENDIR");
	}
	while ((en = readdir(dr)) != 0)
		listing += "<li><a href=\""  + _res + "/" + std::string(en->d_name) +  "\">" + std::string(en->d_name) + "</a></li>\n";
	closedir(dr);
	base = replace(base, "$2", listing);
	_code = 200;
	return (base);
}

std::string Response::replace(std::string source, std::string to_replace, std::string new_value) {
	size_t start_pos = 0;
	while((start_pos = source.find(to_replace, start_pos)) != std::string::npos) {
		source.replace(start_pos, to_replace.length(), new_value);
		start_pos += new_value.length();
	}
	return (source);
}

std::string Response::deleteMethod() {
	if ((getTypeFile(_pathToRes) == 1) && (remove(_pathToRes.c_str()) == 0))
	{
	_code = 204;
		return (createResponse(""));
	}
	_code = 404;
	return (createResponse(getErrorPage()));
}

std::string Response::getMethod() {
	_bodyContent = readFile(_pathToRes);
	if (_code == 403 || _code == 404)
		return (createResponse(getErrorPage()));
	_headers["Content-Type"] = getMIME();
	_headers["Last-Modified"] = getLastModif();
	_code = 200;
	return createResponse(_bodyContent);
}

std::string Response::getLastModif() {
	struct stat buffer;
	struct timezone tz;
	struct timeval t;
	time_t last_date;

	gettimeofday(&t, &tz);
	stat(_pathToRes.c_str(), &buffer);
	last_date = buffer.st_mtime + tz.tz_minuteswest * 60;
	return (formatDate(last_date));
}

std::string Response::postMethod() {
	int fd = -1;
	std::string path;
	std::string name_header;

	if (_uploadPath != "")
		path = _uploadPath;
	else
		path = _pathToRes;
	
	int typeFile;
	typeFile = getTypeFile(path);
	if (typeFile != 0) {
		if ((fd = open(path.c_str(), O_WRONLY | O_TRUNC | O_CREAT, 0644)) == -1) {
			_code = 500;
			return (createResponse(getErrorPage()));
		}
		write(fd, _requestContent.c_str(), _requestContent.length());
		close(fd);
		_code = (typeFile == 1 ? 200 : 201); 
		name_header = (typeFile == 1 ? "Content-Location" : "Location");
		_headers[name_header] = _res;
		return (typeFile == 1 ? createResponse(_requestContent) : createResponse(""));
	}
	_code = 500;
	return (createResponse((getErrorPage())));
}

std::string Response::readFile(std::string filepath) {
	std::stringstream	buf;
	std::string			res;

	if (getTypeFile(filepath) == 1) {
		std::ifstream f(filepath, std::ifstream::binary);
		if (f.is_open() == false)
		{
			_code = 403;
			return ("");
		}
		buf << f.rdbuf();
		res = buf.str();
		f.close();
		return (res);
	}
	_code = 404;
	return ("");
}

int	Response::getTypeFile(std::string filepath) {
	struct stat s;
	if (stat(filepath.c_str(), &s) == 0 ) {
		if (s.st_mode & S_IFREG)
			return 1;
		else
			return 0;
	}
	else
		return -1;
}

void Response::initMIME() {
	_mime["aac"] = "audio/aac";
	_mime["abw"] = "application/x-abiword";
	_mime["arc"] = "application/octet-stream";
	_mime["avi"] = "video/x-msvideo";
	_mime["azw"] = "application/vnd.amazon.ebook";
	_mime["bin"] = "application/octet-stream";
	_mime["bmp"] = "image/bmp";
	_mime["bz"] = "application/x-bzip";
	_mime["bz2"] = "application/x-bzip2";
	_mime["csh"] = "application/x-csh";
	_mime["css"] = "text/css";
	_mime["csv"] = "text/csv";
	_mime["doc"] = "application/msword";
	_mime["docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	_mime["eot"] = "application/vnd.ms-fontobject";
	_mime["epub"] = "application/epub+zip";
	_mime["gif"] = "image/gif";
	_mime["htm"] = "text/html";
	_mime["html"] = "text/html";
	_mime["ico"] = "image/x-icon";
	_mime["ics"] = "text/calendar";
	_mime["jar"] = "application/java-archive";
	_mime["jpeg"] = "image/jpeg";
	_mime["jpg"] = "image/jpeg";
	_mime["js"] = "application/javascript";
	_mime["json"] = "application/json";
	_mime["mid"] = "audio/midi";
	_mime["midi"] = "audio/midi";
	_mime["mpeg"] = "video/mpeg";
	_mime["mpkg"] = "application/vnd.apple.installer+xml";
	_mime["odp"] = "application/vnd.oasis.opendocument.presentation";
	_mime["ods"] = "application/vnd.oasis.opendocument.spreadsheet";
	_mime["odt"] = "application/vnd.oasis.opendocument.text";
	_mime["oga"] = "audio/ogg";
	_mime["ogv"] = "video/ogg";
	_mime["ogx"] = "application/ogg";
	_mime["otf"] = "font/otf";
	_mime["png"] = "image/png";
	_mime["pdf"] = "application/pdf";
	_mime["ppt"] = "application/vnd.ms-powerpoint";
	_mime["pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	_mime["rar"] = "application/x-rar-compressed";
	_mime["rtf"] = "application/rtf";
	_mime["sh"] = "application/x-sh";
	_mime["svg"] = "image/svg+xml";
	_mime["woff"] = "font/woff";
	_mime["woff2"] = "font/woff2";
	_mime["xhtml"] = "application/xhtml+xml";
	_mime["xls"] = "application/vnd.ms-excel";
	_mime["xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	_mime["xml"] = "application/xml";
	_mime["xul"] = "application/vnd.mozilla.xul+xml";
	_mime["zip"] = "application/zip";
	_mime["3gp"] = "audio/3gpp";
	_mime["3g2"] = "audio/3gpp2";
	_mime["7z"] = "application/x-7z-compressed";
	_mime["txt"] = "text/plain";
	_mime["cpp"] = "text/plain";
	_mime["hpp"] = "text/plain";
}

std::string Response::getMIME() {
	std::string ext;
	size_t i;

	i = _pathToRes.size() - 1;
	while (i > 0 && _pathToRes[i] != '.')
		--i;
	ext = std::string(_pathToRes, i + 1, _pathToRes.size() - i);
	if (_mime.count(ext))
		return (_mime[ext]);
	return ("application/octet-stream");
}

void Response::initReasonPhrases() {
	_reasonPhrases[200] = "OK";
	_reasonPhrases[201] = "Created";
	_reasonPhrases[202] = "Accepted";
	_reasonPhrases[204] = "No Content";
	_reasonPhrases[300] = "Multiple Choices";
	_reasonPhrases[301] = "Moved Permanently";
	_reasonPhrases[302] = "Found";
	_reasonPhrases[303] = "See Other";
	_reasonPhrases[307] = "Temporary Redirect";
	_reasonPhrases[400] = "Bad Request";
	_reasonPhrases[401] = "Unauthorized";
	_reasonPhrases[403] = "Forbidden";
	_reasonPhrases[404] = "Not Found";
	_reasonPhrases[405] = "Method Not Allowed";
	_reasonPhrases[413] = "Payload Too Large";
	_reasonPhrases[414] = "URI Too Long";
	_reasonPhrases[500] = "Internal Server Error";
	_reasonPhrases[501] = "Not Implemented";
	_reasonPhrases[502] = "Bad Gateway";
	_reasonPhrases[503] = "Service Unavailable";
	_reasonPhrases[504] = "Gateway Timeout";
	_reasonPhrases[505] = "HTTP Version Not Supported";
}

std::string Response::getReasonPhrase() { return _reasonPhrases[_code]; }

std::string Response::createResponse(std::string body) {
	_response += "HTTP/1.1 ";
	_response += SSTR(_code << " ");
	_response += getReasonPhrase() + "\r\n";
	
	_headers["Content-Length"] = SSTR(body.size());
	_headers["Server"] = "WebServ";
	_headers["Date"] = getCurrentDate();

	std::map<std::string, std::string>::iterator it;
	it = _headers.begin();
	while (it != _headers.end()) {
		_response += it->first + ": " + it->second + "\r\n";
		++it;
	}
	std::ifstream cookie("cookie.txt");
	if (cookie.is_open()) {
		std::stringstream ss;
		ss << cookie.rdbuf();
		if (ss.str().length() > 0)
			_response += ss.str();
		cookie.close();
		remove("cookie.txt");
	}
	_response += "\r\n";
	_response += body;
	return (_response);
}

std::string Response::getCurrentDate(void) {
	struct timeval now;
	struct timezone tz;

	gettimeofday(&now, &tz);
	return (formatDate(now.tv_sec + tz.tz_minuteswest * 60));
}

std::string Response::formatDate(time_t date) {
	char buffer[33];
	struct tm *ts;
	size_t last;

	ts   = localtime(&date);
	last = strftime(buffer, 32, "%a, %d %b %Y %T GMT", ts);
	buffer[last] = '\0';
	return (std::string(buffer));
}

std::string Response::getErrorPage() {
	std::string errorPage;

	errorPage = readFile(_errorPath);
	errorPage = replace(errorPage, "$1", SSTR(_code));
	errorPage = replace(errorPage, "$2", getReasonPhrase());
	return (errorPage);
}

std::string Response::getMethod() const {
	return (_method);
}
