/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artclave <artclave@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 20:06:17 by bperez-a          #+#    #+#             */
/*   Updated: 2024/10/07 07:37:54 by artclave         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParser.hpp"
std::vector<ServerConfig> ConfigParser::parse(const std::string& configFile) {
    std::vector<ServerConfig> servers;
    ServerConfig currentServer;
    std::ifstream file(configFile.c_str());
    if (!file.is_open())
		Utils::error("Unable to open config file: " + configFile, EXIT);
    std::string line;
    bool inServer = false;
    bool inLocation = false;
    bool inCGI = false;
    LocationConfig currentLocation;
    CGIConfig currentCGI;
    int braceCount = 0;
	currentServer.setClientMaxBodySize(-1);

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
                    if (!inCGI) {
                        currentServer.getLocations().push_back(currentLocation);
                    }
                }
                inLocation = true;
                iss >> value;
                if (value == "/cgi-bin") {
                    inCGI = true;
                    currentCGI = CGIConfig();
                } else {
                    inCGI = false;
                    currentLocation = LocationConfig();
                    currentLocation.path = value;
                }
                iss >> value; // Read opening brace
                braceCount++;
            } else if (key == "{") {
                braceCount++;
            } else if (key == "}") {
                braceCount--;
                if (braceCount == 1 && inLocation) {
                    if (inCGI) {
                        currentServer.setCgi(currentCGI);
                        inCGI = false;
                    } else {
                        currentServer.getLocations().push_back(currentLocation);
                    }
                    inLocation = false;
                } else if (braceCount == 0) {
                    servers.push_back(currentServer);
                    currentServer = ServerConfig();
                    inServer = false;
                }
            } else if (inServer) {
                if (inLocation) {
                    if (inCGI) {
                        // Parse CGI-specific configurations
                        if (key == "root") {
                            iss >> currentCGI.root;
                        } else if (key == "cgi_path") {
                            iss >> currentCGI.path;
                        } else if (key == "cgi_ext") {
                            iss >> currentCGI.ext;
                        }
                    } else {
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
                    }
                } else {
                    // Handle server-level configurations
                    if (key == "listen") {
						std::string listen;
                        iss >> listen;
						currentServer.setListen(listen);
                    } else if (key == "server_name") {
						std::string::iterator it1, it2;
						std::vector<std::string> vec;
						int pos1 = line.find("server_name") + sizeof("server_name");
						size_t the_rest = std::string::npos;
                        std::string serverName = line.substr(pos1, the_rest);
						for (it1 = serverName.begin(); it1 < serverName.end(); it1++){
							it2 = std::find(it1, serverName.end(), ' ');
							vec.push_back(std::string(it1, it2));
							it1 = it2;
						}
						currentServer.setServerNames(vec);
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
