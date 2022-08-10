/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fdarrin <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/08 20:32:45 by fdarrin           #+#    #+#             */
/*   Updated: 2021/07/08 20:32:47 by fdarrin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP

#include <string>
#include <iostream>
#include <map>
#include <vector>

class Location {

public:

	std::string modifier;
	std::vector<std::string> locationMethods;
	std::string locationRoot;
	std::string locationIndex;
	std::string locationReturn;
	std::string bufferSize;
	std::string locationCgi;
	std::map<std::string, std::string> information;

	std::string getModifier();
	std::vector<std::string> getLocationMethods(std::string str);
	std::string getLocationRoot();
	std::string getLocationIndex();
	std::string getLocationReturn();
	std::string getBufferSize();
	std::string getLocationCgi();

};

#endif
