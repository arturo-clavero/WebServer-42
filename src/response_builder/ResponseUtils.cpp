/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseUtils.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artclave <artclave@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/09 09:41:35 by bperez-a          #+#    #+#             */
/*   Updated: 2024/10/02 19:56:51 by artclave         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ResponseUtils.hpp"


ResponseUtils::ResponseUtils() {
}

bool ResponseUtils::isRequestTooLarge(const HttpRequest& request, const size_t& clientMaxBodySize) {
	if (clientMaxBodySize > 0 && request.getBody().size() > clientMaxBodySize) {
		return true;
	}
	return false;
}

bool ResponseUtils::isRequestValid(const HttpRequest& request) {
	if (request.getMethod().empty() && request.getPath().empty() && request.getProtocol().empty()) {
		return false;
	}
	return true;
}
bool ResponseUtils::isCGIRequest(const ServerConfig& config, const HttpRequest& request) {
    const CGIConfig& cgiConfig = config.getCgi();
    if (cgiConfig.root.empty()) {
        return false;
    }
    std::string requestPath = request.getPath();
    if (requestPath.compare(0, 8, "/cgi-bin") == 0) {
        // Check if the file has the correct extension
        size_t dotPos = requestPath.find_last_of('.');
        if (dotPos != std::string::npos) {
            std::string extension = requestPath.substr(dotPos);
            if (extension == cgiConfig.ext) {
                return true;
            }
        }
    }
    return false;
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
	if (request.getPath().find(".") != std::string::npos) {
		return IS_FILE;
	}
	else if (request.getPath().find("/") != std::string::npos) {
		return IS_DIRECTORY;
	}
	else {
		return NOT_FOUND;
	}
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

bool ResponseUtils::openFiles(const std::string& folderPath, const std::string& filename, const std::string& content, HttpRequest &request) {
	(void)request;
	(void)content;
	std::string fullPath = folderPath;
    if (fullPath[fullPath.length() - 1] != '/') {
        fullPath += '/';
    }
    fullPath += filename;
    // Open an output file stream
	int fd = open(fullPath.c_str(), O_RDWR | O_CREAT | O_APPEND, 0644);
	if (fd < 0)
		return false;
	request.addPostFileFd(fd);
	request.addPostFileContent(content);
    return true;
}