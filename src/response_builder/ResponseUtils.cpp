/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseUtils.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bperez-a <bperez-a@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/09 09:41:35 by bperez-a          #+#    #+#             */
/*   Updated: 2024/09/09 12:26:08 by bperez-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ResponseUtils.hpp"


ResponseUtils::ResponseUtils() {
}

bool ResponseUtils::isRequestValid(const HttpRequest& request) {
	(void)request;
	return true;
}

bool ResponseUtils::isMethodAllowed(const HttpRequest& request) {
	(void)request;
	return true;
}

LocationConfig ResponseUtils::findLocation(const std::string& path, const ServerConfig& config) {
	size_t max_len = 0;
	LocationConfig bestMatch;
	std::vector<LocationConfig> locations = config.getLocations();

	for (std::vector<LocationConfig>::const_iterator it = locations.begin(); it != locations.end(); ++it) {
		if (path.compare(0, it->path.length(), it->path) == 0) {
			if (it->path.length() > max_len) {
				max_len = it->path.length();
				bestMatch = *it;
			}
		}
	}

	return bestMatch;
}

FileType ResponseUtils::getTargetType(const HttpRequest& request) {
	std::cout << "DEBUG: Entering getTargetType with path: " << request.getPath() << std::endl;

	if (request.getPath().find(".") != std::string::npos) {
		std::cout << "DEBUG: Found a dot in the path, returning IS_FILE" << std::endl;
		return IS_FILE;
	}
	else if (request.getPath().find("/") != std::string::npos) {
		std::cout << "DEBUG: Found a slash in the path, returning IS_DIRECTORY" << std::endl;
		return IS_DIRECTORY;
	}
	else {
		std::cout << "DEBUG: No dot or slash found, returning NOT_FOUND" << std::endl;
		return NOT_FOUND;
	}
}

std::string ResponseUtils::buildBodyFromFile(const ServerConfig& config, const std::string& path) {
	(void)config;
	std::ifstream file(path.c_str());
	std::string body;
	if (file.is_open()) {
		std::string line;
		while (getline(file, line)) {
			body += line + "\n";
		}
	}
	return body;
}