/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fdarrin <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/08 20:32:45 by fdarrin           #+#    #+#             */
/*   Updated: 2021/07/08 20:32:47 by fdarrin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <iostream>
# include <vector>
# include "Location.hpp"


class Config {

public:

	std::string serverName;
	std::string listenIp;
	uint16_t listenPort;
	std::vector<std::string> serverMethods;
	std::string serverRoot;
	std::string serverIndex;
	std::string serverAutoindex;
	std::string bufferSize;
	std::string errorPage;
	std::map<std::string, std::string> serverInformation;
	std::map<std::string, Location> location;

	std::string getServerName();
	std::string getListenIp();
	uint16_t 	getListenPort();
	std::vector<std::string> getServerMethods(std::string str);
	std::string getServerRoot();
	std::string getServerIndex();
	std::string getServerAutoindex();
	std::string getBufferSize();
	std::string getErrorPage();
	std::map<std::string, std::string> getServerInformation();
	std::map<std::string, Location> getLocation();

};


#endif
