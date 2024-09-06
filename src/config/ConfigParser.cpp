/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bperez-a <bperez-a@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 20:06:17 by bperez-a          #+#    #+#             */
/*   Updated: 2024/09/06 10:10:08 by bperez-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParser.hpp"

ServerConfig ConfigParser::parse(const std::string& configFile) {
    ServerConfig server;
    std::ifstream file(configFile.c_str());
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open config file: " + configFile);
    }

    std::string line;
    bool inServer = false;
    bool inLocation = false;
    LocationConfig currentLocation;
    int braceCount = 0;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key;
        std::string value;

        if (iss >> key) {
            if (key == "server" && iss >> value && value == "{") {
                inServer = true;
                braceCount++;
            } else if (key == "location" && inServer) {
                if (inLocation) {
                    server.getLocations().push_back(currentLocation);
                }
                inLocation = true;
                currentLocation = LocationConfig();
                iss >> currentLocation.path >> value; // Read path and opening brace
                braceCount++;
            } else if (key == "{") {
                braceCount++;
            } else if (key == "}") {
                braceCount--;
                if (braceCount == 1 && inLocation) {
                    server.getLocations().push_back(currentLocation);
                    inLocation = false;
                } else if (braceCount == 0) {
                    inServer = false;
                }
            } else if (inServer) {
                if (inLocation) {
                    // Parse location-specific configurations
                    if (key == "allow_methods") {
                        std::string method;
                        while (iss >> method) {
                            currentLocation.allowMethods.push_back(method);
                        }
                    } else if (key == "autoindex") {
                        std::string value;
                        iss >> value;
                        currentLocation.autoindex = (value == "on");
                    } else if (key == "root") {
                        iss >> currentLocation.root;
                    } else if (key == "index") {
                        iss >> currentLocation.index;
                    } else if (key == "directory_listing") {
                        // Handle directory_listing if needed
                    }
                } else {
                    // Handle server-level configurations
                    if (key == "listen") {
                        int listen;
                        iss >> listen;
                        server.setListen(listen);
                    } else if (key == "host") {
                        std::string host;
                        iss >> host;
                        server.setHost(host);
                    } else if (key == "server_name") {
                        std::string serverName;
                        iss >> serverName;
                        server.setServerName(serverName);
                    } else if (key == "error_page") {
                        int errorCode;
                        std::string errorPage;
                        if (iss >> errorCode >> errorPage) {
                            std::map<int, std::string> errorPages = server.getErrorPages();
                            errorPages[errorCode] = errorPage;
                            server.setErrorPages(errorPages);
                        }
                    } else if (key == "client_max_body_size") {
                        std::string sizeStr;
                        iss >> sizeStr;
                        size_t clientMaxBodySize = static_cast<size_t>(std::atof(sizeStr.substr(0, sizeStr.length() - 1).c_str()) * 1024 * 1024);
                        server.setClientMaxBodySize(clientMaxBodySize);
                    } else if (key == "root") {
                        std::string root;
                        iss >> root;
                        server.setRoot(root);
                    } else if (key == "index") {
                        std::string index;
                        iss >> index;
                        server.setIndex(index);
                    }
                }
            }
        }
    }
    return server;
}
