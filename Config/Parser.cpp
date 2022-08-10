/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fdarrin <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/08 20:33:05 by fdarrin           #+#    #+#             */
/*   Updated: 2021/07/08 20:33:08 by fdarrin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

int Parser::findServer(std::string const & str, int index) {

	int i = 0;
	while ((i = str.find("server", index)) != std::string::npos) {
		if (((i == 0) && checkChar("{\n\t\v\r ", str[i+6])) || (checkChar
		("}\n\t\v\r ", str[i-1]) && checkChar("{\n\t\v\r ", str[i+6])))
			return i;
		index = i + 1;
	}
	return i;

}

bool Parser::checkChar(std::string const & str, char c) {

	for (int i = 0; str[i]; ++i)
		if (str[i] == c)
			return true;
	return false;

}

void Parser::parsServer(std::string const & srv) {

	Config server;
	checkBrackets(srv);
	std::string str = std::string(srv.begin()+srv.find("{")+1, srv.begin()+srv.rfind("}"));
	int shift;
	if (findLocation(str, 0) != std::string::npos)
		shift = findLocation(str, 0);
	else
		shift = str.length();
	serverInformation(std::string(str.begin(), str.begin()+ shift), &server);
	int index = 0;
	int i = 0;
	int j = 0;
	while ((i = findLocation(str, index)) != std::string::npos) {
		int brackets = 0;
		for (j = i; j < str.length() && str[j] != '{'; ++j);
		if  (str[j] == '{') {
			brackets++;
			j++;
			for ( ; j < str.length() && brackets != 0; ++j)
				brackets += (str[j] == '{') ? 1 : ((str[j] == '}') ? -1 : 0);
			if (brackets == 0)
				parsLocation(std::string(str.begin()+i+9, str.begin() + j), &server, "");
		}
		index = j;
	}
	servers.push_back(server);

}

bool Parser::checkBrackets(std::string const & str) {

	int start = str.find("{");
	int finish = str.rfind("}");
	if (start == std::string::npos || finish == std::string::npos)
		throw "Not enough brackets";
	for (int i = 0; i < start; ++i)
		if (!checkChar("\n\t\v\r ", str[i]))
			throw "Extra char before bracket";
	for (int i = finish + 1; i < str.length(); ++i)
		if (!checkChar("\n\t\v\r ", str[i]))
			throw "Extra char after bracket";
	int count = 0;
	for (int i = start + 1; i < finish; ++i) {
		count += (str[i] == '{') ? 1 : ((str[i] == '}') ? -1 : 0);
	}
	if (count != 0)
		throw "Bad brackets";
	return (1);

}

int Parser::findLocation(std::string const & str, int index) {

	int i = 0;
	while ((i = str.find("location ", index)) != std::string::npos) {
		if ((i == 0) || (checkChar("\n\t\v\r ", str[i-1])))
			return (i);
		index = i + 1;
	}
	return (i);

}

void Parser::serverInformation(std::string const & str, Config * server) {

	std::vector<std::string> info = split(remove(std::string(str), "\t\v\r"));
	for (int i = 0; i < info.size(); ++i) {
		int j = info[i].find(" ");
		if (j != std::string::npos)
			server->serverInformation[std::string(info[i].begin(), info[i]
			.begin()+j)] = std::string(info[i].begin()+j+1, info[i].end());
	}
	if (server->serverInformation.count("server_name"))
		server->serverName = std::string(server->serverInformation["server_name"].begin(), server->serverInformation["server_name"].end());
	if (!server->serverInformation.count("listen"))
		throw "Listen not found";
	else {
		int f = server->serverInformation["listen"].find(":", 0);
		server->listenIp = std::string(server->serverInformation["listen"].begin(), server->serverInformation["listen"].begin()+f);
		server->listenPort = std::stoi(std::string(server->serverInformation["listen"].begin()+f+1, server->serverInformation["listen"].end()));
	}
	if (server->serverInformation.count("allow_methods"))
		parsServerMethods(server->serverInformation["allow_methods"], server);

	if (server->serverInformation.count("root"))
		server->serverRoot = server->serverInformation["root"];

	if (server->serverInformation.count("index"))
		server->serverIndex = server->serverInformation["index"];

	if (server->serverInformation.count("autoindex"))
		server->serverAutoindex = server->serverInformation["autoindex"];

	if (server->serverInformation.count("client_body_buffer_size"))
		server->bufferSize = server->serverInformation["client_body_buffer_size"];

	if (server->serverInformation.count("error_page"))
		server->errorPage = server->serverInformation["error_page"];
}

std::vector<std::string> Parser::split(const std::string & str) {

	std::vector<std::string> info;
	std::string item;
	std::stringstream ss;
	ss.str(str);
	while (std::getline(ss, item, '\n')) {
		if (item.size() > 0)
			info.push_back(item);
	}
	return info;

}

std::string Parser::remove(std::string item, std::string const & s) {

	for (int i = 0; i < s.size(); ++i) {
		std::string::iterator it = std::remove(item.begin(), item.end(), s[i]);
		item.erase(it, item.end());
	}
	return item;

}

void Parser::parsServerMethods(std::string info, Config *server) {

	std::string item;
	std::stringstream ss;
	ss.str(info);
	while (std::getline(ss, item, ' ')) {
		if (item.size() > 0)
			server->serverMethods.push_back(item);
	}

}

void Parser::parsLocation(std::string const & loc, Config * server, std::string
const & mod) {

	Location location;
	location.modifier = remove(std::string(loc.begin(), loc.begin()+loc.find
	("{")), "\n\t\v\r ");
	if	(location.modifier[0] != '/')
		throw "Bad location modifier";
	else
		location.modifier =  mod + location.modifier;
	std::string str = std::string(loc.begin()+loc.find("{"), loc.begin()+loc.rfind("}")+1);
	checkBrackets(str);
	str = std::string(str.begin()+str.find("{")+1, str.begin()+str.rfind("}"));
	int shift;
	if (findLocation(str, 0) != std::string::npos)
		shift = findLocation(str, 0);
	else
		shift = str.length();
	locationInformation(std::string(str.begin(), str.begin()+ shift), &location);
	int index = 0;
	int i = 0;
	int j = 0;
	while ((i = findLocation(str, index)) != std::string::npos) {
		j = 0;
		int brackets = 0;
		for (j = i; j < str.length() && str[j] != '{'; ++j);
		if  (str[j] == '{') {
			brackets++;
			j++;
			for ( ; j < str.length() && brackets != 0; ++j)
				brackets += (str[j] == '{') ? 1 : ((str[j] == '}') ? -1 : 0);
			if (brackets == 0)
				parsLocation(std::string(str.begin()+i+9, str.begin() + j),
						server, location.modifier);
		}
		index = j;
	}
	server->location[location.modifier] = location;

}

void Parser::checkServer(void) {

	for (int i = 0; i < servers.size(); ++i) {
		std::string path;
		if (servers[i].serverInformation.count("root") == 0)
			throw "Root not found";
		else
			path = servers[i].serverInformation["root"];
		if (servers[i].serverInformation["root"][0] != '.')
			throw "Bad root";
		else if (checkPath(servers[i].serverInformation["root"].c_str()) != 1)
			throw "Bad path";
		if (servers[i].serverInformation.count("index") == 1) {
			path += "/" + (servers[i]).serverInformation["index"];
			if (checkPath(path.c_str()) != 2)
				throw "Bad index";
		}
		if (servers[i].serverInformation.count("cgi_bin") == 1)
			throw "Bad cgi_bin";
		if (servers[i].serverInformation.count("error_page") == 1)
			if (checkPath(servers[i].serverInformation["error_page"].c_str()) != 2)
				throw "Bad error_page";
		checkLocation(&(servers[i]));
	}

}

int Parser::checkPath(const char * path) {

	struct stat info;
	int statRC = stat(path, &info);
	if (statRC) {
		if (errno == ENOENT || errno == ENOTDIR)
			return (0);
		return -1;
	}
	if (!(info.st_mode & S_IFDIR))
		return 2;
	return 1;

}

void Parser::checkLocation(Config * server) {

	for (std::map<std::string, Location>::iterator it = server->location
			.begin(); it != server->location.end(); ++it) {
		std::string path;
		if (it->second.information.count("root") == 1) {
			path = it->second.information["root"];
			if (path[0] != '.')
				throw "Bad root";
			if (checkPath(path.c_str()) != 1)
				throw "Bad path";
		}
		else
			path = "." + it->first;
		if (it->second.information.count("index") == 1) {
			path += "/" + it->second.information["index"];
			if (checkPath(path.c_str()) != 2)
				throw "Bad index";
		}
		if (it->second.information.count("cgi_bin") == 1) {
			path += "/" + it->second.information["cgi_bin"];
		}
	}

}

void Parser::locationInformation(std::string const & str, Location * location) {

	std::vector<std::string> info = split(remove(std::string(str), "\t\v\r"));
	for (int i = 0; i < info.size(); ++i) {
		int g = info[i].find(" ");
		if (g != std::string::npos)
			location->information[std::string(info[i].begin(), info[i].begin()+g)] = std::string(info[i].begin()+g+1, info[i].end());
	}

	if (location->information.count("root"))
		location->locationRoot = location->information["root"];

	if (location->information.count("allow_methods"))
		parsLocationMethods(location->information["allow_methods"], location);

	if (location->information.count("index"))
		location->locationIndex = location->information["index"];

	if (location->information.count("return"))
		location->locationReturn = location->information["return"];

	if (location->information.count("client_body_buffer_size"))
		location->bufferSize = location->information["client_body_buffer_size"];

	if (location->information.count("cgi_bin"))
		location->locationCgi = location->information["cgi_bin"];

}

void Parser::parsLocationMethods(std::string info, Location * location) {

	std::string item;
	std::stringstream ss;
	ss.str(info);
	while (std::getline(ss, item, ' ')) {
		if (item.size() > 0)
			location->locationMethods.push_back(item);
	}

}

void Parser::parser(const std::string &path) {

	std::ifstream file(path);
	if (!file.is_open())
		throw "Config not opened";
	std::stringstream ss;
	ss << file.rdbuf();
	std::string const & str = ss.str();
	int count = 0;
	if (findServer(str, count) == std::string::npos)
		throw "Server not found";
	int i = 0;
	int j = 0;
	while ((i = findServer(str, count)) != std::string::npos) {
		if ((j = findServer(str, i+1)) != std::string::npos)
			parsServer(std::string(str.begin()+i+6, str.begin()+j));
		else
			parsServer(std::string(str.begin()+i+6, str.end()));
		count = i + 1;
	}
	checkServer();
	file.close();

}

std::vector<Config> Parser::getServers() {

	return servers;

}
