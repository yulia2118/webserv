#include "Request.hpp"

std::vector<std::string> Request::splitLine(std::string buffer) {
	std::vector<std::string> table(0);
	std::stringstream ss(buffer);
	std::string item;
	while (std::getline(ss, item, '\n')) {
		table.push_back(removeDelim(item, "\t\r\v"));
	}
	return (table);
}

std::vector<std::string> Request::splitFirst(std::string buffer) {
	std::vector<std::string> table;
	std::stringstream ss(buffer);
	std::string item;
	while (std::getline(ss, item, ' '))
		table.push_back(item);
	return (table);
}

std::vector<std::string> Request::splitHeaders(std::string buffer) {
	std::vector<std::string> table;
	std::stringstream ss(buffer);
	std::string item;
	char delim = ':';
	while (std::getline(ss, item, delim)) {
		std::string::iterator end_pos = std::remove(item.begin(), item.end(), ' ');
		item.erase(end_pos, item.end());
		table.push_back(item);
		delim = '\0';
	}	
	return (table);
}

std::string Request::removeDelim(std::string item, std::string const &set) {
	for (unsigned long i = 0; i < set.size(); ++i) {
		std::string::iterator end_pos = std::remove(item.begin(), item.end(), set[i]);
		item.erase(end_pos, item.end());
	}
	return (item);
}

Request::Request(char *buffer, Config *server) {
	std::vector<std::string> table(0);
	std::vector<std::string> first_line;
	std::vector<std::string> other_line;
	_allowMethods = "";
	table = splitLine(std::string(buffer));
	unsigned long i = 0;
	if (!table.empty()) {
		first_line = splitFirst(table[i].data());
		if (first_line.size() == 3) {
			method = removeDelim(first_line[0], "\t\v\r");
			path = removeDelim(first_line[1], "\t\v\r");
			reqPath = removeDelim(first_line[1], "\t\v\r");
			version = removeDelim(first_line[2], "\t\v\r");
		}
		for (i = 1; table[i] != "" && i < table.size(); ++i) {
			other_line = splitHeaders(table[i].data());
			if (other_line.size() == 2)
				headers[other_line[0]] = other_line[1];
		}
		if (table[i] == "") {
			i++;
			for ( ; i < table.size(); ++i) {
				body += table[i] + "\n";
			}
		}
	}
	hostString = server->listenIp;
	port = server->listenPort;

	pathPost = "./";
	autoindex = false;
	isCgi = false;

	statusCode = checkValid(server);
	if (server->serverInformation.count("error_page") == 1)
		errorPath = server->serverInformation["error_page"];
	else
		errorPath = "./html/www/error.html";
}

int	Request::dirExists(const char* const path) {
	struct stat info;

	int statRC = stat(path, &info);
	if( statRC != 0 )
	{
		if (errno == ENOENT)
			return (0);
		if (errno == ENOTDIR)
			return (0);
		return (-1);
	}
	return ((info.st_mode & S_IFDIR ) ? 1 : 2);
}

int Request::checkValidPath(Config *server)
{
	std::string url = "/";
	for (std::map<std::string, Location>::iterator it = server->location.begin();
		it != server->location.end(); ++it)
	{
		if (path.find(it->first, 0) == 0)
			if (it->first.length() >= url.length())
				url = it->first;
	}
	if (server->serverInformation.count("autoindex") == 1)
		if (server->serverInformation["autoindex"] == "on")
			autoindex = true;
	if (url == "/") {
		_allowMethods = server->serverInformation["allow_methods"];
		if (path == url || path == url + "/") {
			if (server->serverInformation.count("index") == 1 && method == "GET")
				path = path + "/" + server->serverInformation["index"];
		}
		if (server->serverInformation.count("root") == 1)
			path = server->serverInformation["root"] + path;
		if (dirExists(path.c_str()) <= 0)
			return (404);
		if (server->serverInformation.count("allow_methods") == 1)
			if (server->serverInformation["allow_methods"].find(method) == std::string::npos )
				return (405);
		if (server->serverInformation.count("client_body_buffer_size") == 1) {
			if (std::stoi(server->serverInformation["client_body_buffer_size"]) < (int)body.size())
				return (413);
		}
		else {
			if (12656974 < body.size())
				return (413);
		}
	}
	else
	{
		_allowMethods = server->location[url].information["allow_methods"];
		std::string  ending = std::string(path.begin() + url.size(), path.end());
		if (server->location[url].information.count("return") == 1) {
			path = server->location[url].information["return"] + ending;
			return (301);
		}
		if (path == url || path == url + "/") {
			if (server->location[url].information.count("index") == 1 && method == "GET")
				path = path + "/" + server->location[url].information["index"];
		}
		if (server->location[url].information.count("root") == 1)
				path = server->location[url].information["root"] + ending;
		if (server->location[url].information.count("cgi_bin") == 0)
			if (dirExists(path.c_str()) <= 0)
				return (404);
		if (server->location[url].information.count("allow_methods") == 1)
			if (server->location[url].information["allow_methods"].find(method) == std::string::npos)
				return (405);
		if (server->location[url].information.count("client_body_buffer_size") == 1) {
			if (std::stoi(server->location[url].information["client_body_buffer_size"]) <
				(int)body.size())
				return (413);
		}
		else {
			if (12656974 < body.size())
				return (413);
		}
		if (server->location[url].information.count("cgi_bin") == 1) {
			if (!(method == "GET" || method == "POST"))
				return (405);
			if (method == "GET")
				cgiArg = ending;
			if (method == "POST")
				cgiArg = body;
			isCgi = true;
			if (server->location[url].information.count("root") == 1)
				path = server->location[url].information["root"] + "/" +
				removeDelim(server->location[url].information["cgi_bin"], " \t\v\r");
			if (dirExists(path.c_str()) <= 0)
				return (404);
		}
	}
	removeDuplicateSlash();
	return (0);
}

void  Request::removeDuplicateSlash() {
	unsigned long idx;
	while((idx = path.find("//")) != std::string::npos)
	{
		path.erase(idx, 1);
	}
	while((idx = reqPath.find("//")) != std::string::npos)
	{
		reqPath.erase(idx, 1);
	}
	if (path[path.size()-1] == '/')
		path.erase(path.size()-1, 1);
	if (reqPath[reqPath.size()-1] == '/')
		reqPath.erase(reqPath.size()-1, 1);
}

int Request::checkValid(Config *server) {
	if (!(method == "GET" || method == "POST" || method == "DELETE"))
		return (405);
	if (!(version == "HTTP/1.1"))
		return (505);
	if (!(path[0] == '/'))
		return (505);
	return (checkValidPath(server));
}

std::string Request::getAllowMethods() {
	return _allowMethods;
}
