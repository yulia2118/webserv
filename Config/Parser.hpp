/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fdarrin <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/08 20:32:45 by fdarrin           #+#    #+#             */
/*   Updated: 2021/07/08 20:32:47 by fdarrin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP

# include <fstream>
# include <sstream>
# include <vector>
# include <map>
# include <algorithm>
# include <sys/stat.h>
# include <sys/types.h>
# include "Config.hpp"

class Parser {

private:

	int findServer(std::string const & str, int index);
	bool checkChar(std::string const & str, char c);
	void parsServer(std::string const & srv);
	bool checkBrackets(std::string const & str);
	int findLocation(std::string const & str, int index);
	void serverInformation(std::string const & str, Config * server);
	std::vector<std::string> split(const std::string & str);
	std::string remove(std::string item, std::string const & s);
	void parsServerMethods(std::string info, Config * server);
	void parsLocationMethods(std::string info, Location * location);
	void parsLocation(std::string const & loc, Config * server,
			std::string const & mod);
	void locationInformation(std::string const & str, Location * location);
	void checkServer(void);
	int checkPath(const char * path);
	void checkLocation(Config * server);

public:

	std::vector<Config> servers;

	void parser(const std::string &path);
	std::vector<Config> getServers();

};

#endif
