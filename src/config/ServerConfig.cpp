/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: valeriafedorova <valeriafedorova@studen    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 14:00:00 by bperez-a          #+#    #+#             */
/*   Updated: 2024/09/09 11:18:21 by bperez-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConfig.hpp"

// Default constructor
ServerConfig::ServerConfig() { }


// Copy constructor
ServerConfig::ServerConfig(const ServerConfig& other)
    : port(other.port),
      host(other.host),
      serverName(other.serverName),
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
        port = other.port;
        host = other.host;
        serverName = other.serverName;
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
int ServerConfig::getPort() const { return port; }
std::string  ServerConfig::getHost() const {
	return  0;
	//TODO

 }
const std::string& ServerConfig::getServerName() const { return serverName; }
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
void ServerConfig::setPort(int value) { port  = value; }
void ServerConfig::setHost(const std::string& value) { host = value; }
void ServerConfig::setServerName(const std::string& value) { serverName = value; }
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
    std::cout << "Port: " << port << std::endl;
    std::cout << "Host: " << host << std::endl;
    std::cout << "Server Name: " << serverName << std::endl;
    std::cout << "Client Max Body Size: " << clientMaxBodySize << " bytes" << std::endl;
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
