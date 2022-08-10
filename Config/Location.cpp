/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fdarrin <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/08 20:33:05 by fdarrin           #+#    #+#             */
/*   Updated: 2021/07/08 20:33:08 by fdarrin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

std::string Location::getModifier() {

	return modifier;

}

std::vector<std::string> Location::getLocationMethods(std::string str) {

	return locationMethods;

}

std::string Location::getLocationRoot() {

	return locationRoot;

}

std::string Location::getLocationIndex() {

	return locationIndex;

}

std::string Location::getLocationReturn() {

	return locationReturn;

}

std::string Location::getBufferSize() {

	return bufferSize;

}

std::string Location::getLocationCgi() {

	return locationCgi;

}
