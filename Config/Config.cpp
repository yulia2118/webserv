/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fdarrin <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/08 20:33:05 by fdarrin           #+#    #+#             */
/*   Updated: 2021/07/08 20:33:08 by fdarrin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"


std::string Config::getServerName() {

	return serverName;

}

std::string Config::getListenIp() {

	return listenIp;

}

uint16_t Config::getListenPort() {

	return listenPort;

}

std::vector<std::string> Config::getServerMethods(std::string str) {

	return serverMethods;

}

std::string Config::getServerRoot()
{
	return serverRoot;

}

std::string Config::getServerIndex() {

	return serverIndex;

}

std::string Config::getServerAutoindex()
{
	return serverAutoindex;

}

std::string Config::getBufferSize() {

	return bufferSize;

}

std::string Config::getErrorPage() {

	return errorPage;

}

std::map<std::string, std::string> Config::getServerInformation() {

	return serverInformation;

}

std::map<std::string, Location> Config::getLocation() {

	return location;

}
