/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artclave <artclave@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 20:06:17 by bperez-a          #+#    #+#             */
/*   Updated: 2024/09/09 11:15:58 by bperez-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParser.hpp"
std::vector<ServerConfig> ConfigParser::parse(const std::string& configFile) {
    std::vector<ServerConfig> servers;
    ServerConfig currentServer;
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
                if (inServer) {
                    servers.push_back(currentServer);
                    currentServer = ServerConfig();
                }
                inServer = true;
                braceCount = 1;
            } else if (key == "location" && inServer) {
                if (inLocation) {
                    currentServer.getLocations().push_back(currentLocation);
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
                    currentServer.getLocations().push_back(currentLocation);
                    inLocation = false;
                } else if (braceCount == 0) {
                    servers.push_back(currentServer);
                    currentServer = ServerConfig();
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
                        int port;
                        iss >> port;
                        currentServer.setPort(port);
                    } else if (key == "host") {
                        std::string host;
                        iss >> host;
                        currentServer.setHost(host);
                    } else if (key == "server_name") {
                        std::string serverName;
                        iss >> serverName;
                        currentServer.setServerName(serverName);
                    } else if (key == "error_page") {
                        std::string errorCode;
                        std::string errorPage;
                        if (iss >> errorCode >> errorPage) {
                            std::map<std::string, std::string> errorPages = currentServer.getErrorPages();
                            errorPages[errorCode] = errorPage;
                            currentServer.setErrorPages(errorPages);
                        }
                    } else if (key == "client_max_body_size") {
                        std::string sizeStr;
                        iss >> sizeStr;
                        size_t clientMaxBodySize = static_cast<size_t>(std::atof(sizeStr.substr(0, sizeStr.length() - 1).c_str()) * 1024 * 1024);
                        currentServer.setClientMaxBodySize(clientMaxBodySize);
                    } else if (key == "root") {
                        std::string root;
                        iss >> root;
                        currentServer.setRoot(root);
                    } else if (key == "index") {
                        std::string index;
                        iss >> index;
                        currentServer.setIndex(index);
                    }
                }
            }
        }
    }

    if (inServer) {
        servers.push_back(currentServer);
    }

    return servers;
}
