/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artclave <artclave@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/09 14:12:10 by bperez-a          #+#    #+#             */
/*   Updated: 2024/10/02 19:50:35 by artclave         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "ServerConfig.hpp"

// Default constructor
ServerConfig::ServerConfig() { }

// Copy constructor
ServerConfig::ServerConfig(const ServerConfig& other)
    : listen(other.listen),
      serverNames(other.serverNames),
      errorPages(other.errorPages),
      clientMaxBodySize(other.clientMaxBodySize),
      root(other.root),
      index(other.index),
      locations(other.locations),
      cgi(other.cgi) {
}

// Copy assignment operator
ServerConfig& ServerConfig::operator=(const ServerConfig& other) {
    if (this != &other) {
		listen = other.listen;
        serverNames = other.serverNames;
        errorPages = other.errorPages;
        clientMaxBodySize = other.clientMaxBodySize;
        root = other.root;
        index = other.index;
        locations = other.locations;
        cgi = other.cgi;
    }
    return *this;
}

// Destructor
ServerConfig::~ServerConfig() {
}

// Getters
const std::string& ServerConfig::getListen() const { return listen; } 
const std::vector<std::string>& ServerConfig::getServerNames() const { return serverNames; }
const std::map<std::string, std::string>& ServerConfig::getErrorPages() const { return errorPages; }
size_t ServerConfig::getClientMaxBodySize() const { return clientMaxBodySize; }
const std::string& ServerConfig::getRoot() const { return root; }
const std::string& ServerConfig::getIndex() const { return index; }
std::vector<LocationConfig>& ServerConfig::getLocations() { return locations; }
const std::vector<LocationConfig>& ServerConfig::getLocations() const { return locations; }
const CGIConfig& ServerConfig::getCgi() const { return cgi; }
int ServerConfig::getListenSocket() const { return listen_socket; }
const struct sockaddr_in& ServerConfig::getAddress() const { return address; }
// Setters
void ServerConfig::setListen(const std::string& value) { listen = value;} 
void ServerConfig::setServerNames(const std::vector<std::string>& value) { serverNames = value; }
void ServerConfig::setErrorPages(const std::map<std::string, std::string>& value) { errorPages = value; }
void ServerConfig::setClientMaxBodySize(size_t value) { clientMaxBodySize = value; }
void ServerConfig::setRoot(const std::string& value) { root = value; }
void ServerConfig::setIndex(const std::string& value) { index = value; }
void ServerConfig::setLocations(const std::vector<LocationConfig>& value) { locations = value; }
void ServerConfig::setCgi(const CGIConfig& value) { cgi = value; }
void ServerConfig::setListenSocket(int value) { listen_socket = value; }
void ServerConfig::setAddress(const struct sockaddr_in& value) { address = value; }
void ServerConfig::print() const {
    std::cout << "Server Configuration:" << std::endl;
	std::cout << "Listen: " << listen << std::endl;
	std::cout << "Server Names: ";
	for (int i = 0; i < static_cast<int>(serverNames.size()); i++)
		std::cout<<serverNames[i]<<" ";
	std::cout << std::endl;
    std::cout << "Client Max Body Size: " << clientMaxBodySize << " bytes" << std::endl;
    std::cout << "Root: " << root << std::endl;
    std::cout << "Index: " << index << std::endl;    std::cout << "Client Max Body Size: " << clientMaxBodySize << " bytes" << std::endl;
    std::cout << "Root: " << root << std::endl;
    std::cout << "Index: " << index << std::endl;

    std::cout << "\nError Pages:" << std::endl;
    std::map<int, std::string>::const_iterator it;
    for (std::map<std::string, std::string>::const_iterator it = errorPages.begin(); it != errorPages.end(); ++it) {
        std::cout << "  " << it->first << ": " << it->second << std::endl;
    }
    std::cout << "\nLocations:" << std::endl;
    std::vector<LocationConfig>::const_iterator locIt;
    for (locIt = locations.begin(); locIt != locations.end(); ++locIt) {
        std::cout << "  Path: " << locIt->path << std::endl;
        std::cout << "    Root: " << locIt->root << std::endl;
        std::cout << "    Autoindex: " << (locIt->autoindex ? "on" : "off") << std::endl;
        std::cout << "    Allow Methods: ";
        std::vector<std::string>::const_iterator methodIt;
        for (methodIt = locIt->allowMethods.begin(); methodIt != locIt->allowMethods.end(); ++methodIt) {
            std::cout << *methodIt << " ";
        }
        std::cout << std::endl;
        std::cout << "    Index: " << locIt->index << std::endl;
        std::cout << "    Return URL: " << locIt->return_url << std::endl;
        std::cout << "    Alias: " << locIt->alias << std::endl;
    }

    std::cout << "\nCGI Configuration:" << std::endl;
    std::cout << "  Root: " << cgi.root << std::endl;
    std::cout << "  Path: " << cgi.path << std::endl;
    std::cout << "  Extension: " << cgi.ext << std::endl;
}
