/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseUtils.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bperez-a <bperez-a@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/09 09:41:35 by bperez-a          #+#    #+#             */
/*   Updated: 2024/09/11 14:11:53 by bperez-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ResponseUtils.hpp"


ResponseUtils::ResponseUtils() {
}

bool ResponseUtils::isRequestValid(const HttpRequest& request) {
	if (request.getMethod().empty() && request.getPath().empty() && request.getProtocol().empty()) {
		return false;
	}
	return true;
}

bool ResponseUtils::isMethodAllowed(const HttpRequest& request, const LocationConfig& location) {

	std::vector<std::string>::const_iterator it;
	for (it = location.allowMethods.begin(); it != location.allowMethods.end(); ++it) {
		if (*it == request.getMethod()) {
			return true;
		}
	}
	return false;
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


std::string ResponseUtils::getContentType(const std::string& path) {
    const char* extensions[] = {
        ".html", ".htm", ".txt", ".css", ".js", ".json",
        ".png", ".jpg", ".jpeg", ".gif", ".bmp", ".ico",
        ".pdf", ".mp4", ".mpeg", ".mov", ".avi", ".wmv",
        ".mp3", ".wav", ".ogg",
        ".xml", ".zip", ".tar", ".gz",
        ".doc", ".docx", ".xls", ".xlsx", ".ppt", ".pptx",
        ".svg", ".webp", ".tiff", ".csv"
    };
    const char* mimeTypes[] = {
        "text/html", "text/html", "text/plain", "text/css", "application/javascript", "application/json",
        "image/png", "image/jpeg", "image/jpeg", "image/gif", "image/bmp", "image/x-icon",
        "application/pdf", "video/mp4", "video/mpeg", "video/quicktime", "video/x-msvideo", "video/x-ms-wmv",
        "audio/mpeg", "audio/wav", "audio/ogg",
        "application/xml", "application/zip", "application/x-tar", "application/gzip",
        "application/msword", "application/vnd.openxmlformats-officedocument.wordprocessingml.document",
        "application/vnd.ms-excel", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet",
        "application/vnd.ms-powerpoint", "application/vnd.openxmlformats-officedocument.presentationml.presentation",
        "image/svg+xml", "image/webp", "image/tiff", "text/csv"
    };
    const int numTypes = sizeof(extensions) / sizeof(extensions[0]);

    size_t dotPos = path.find_last_of('.');
    if (dotPos != std::string::npos) {
        std::string ext = path.substr(dotPos);
        for (int i = 0; i < numTypes; ++i) {
            if (strcasecmp(ext.c_str(), extensions[i]) == 0) {
                return mimeTypes[i];
            }
        }
    }
    return "application/octet-stream";
}