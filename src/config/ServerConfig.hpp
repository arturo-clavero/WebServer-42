/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bperez-a <bperez-a@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 13:38:57 by bperez-a          #+#    #+#             */
/*   Updated: 2024/10/01 12:39:56 by bperez-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_CONFIG_HPP
#define SERVER_CONFIG_HPP

#include "includes.hpp"

struct LocationConfig {
    std::string path;
    std::string root;
    bool autoindex;
    std::vector<std::string> allowMethods;
    std::string index;
    std::string return_url;
    std::string alias;
};

struct CGIConfig {
    std::string root;
    std::string path;
    std::string ext;
};

class ServerConfig {
public:
    ServerConfig();
    ServerConfig(const ServerConfig& other);
    ServerConfig& operator=(const ServerConfig& other);
    ~ServerConfig();

    //int getPort() const;
   // std::string getHost() const;
	const std::string& getListen() const;  //for network version 1  
    const std::vector<std::string>& getServerNames() const;
    const std::map<std::string, std::string>& getErrorPages() const;
    size_t getClientMaxBodySize() const;
    const std::string& getRoot() const;
    const std::string& getIndex() const;
    std::vector<LocationConfig>& getLocations();
    const std::vector<LocationConfig>& getLocations() const;
    const CGIConfig& getCgi() const;
	int getListenSocket() const;
	const struct sockaddr_in& getAddress() const;
	
   // void setPort(int value);
   // void setHost(const std::string& value);
	void setListen(const std::string& value);  //for network version 1 
    void setServerNames(const std::vector<std::string>& value);
    void setErrorPages(const std::map<std::string, std::string>& value);
    void setClientMaxBodySize(size_t value);
    void setRoot(const std::string& value);
    void setIndex(const std::string& value);
    void setLocations(const std::vector<LocationConfig>& value);
    void setCgi(const CGIConfig& value);
	void setListenSocket(int value);
	void setAddress(const struct sockaddr_in& value);
	void print() const;	

private:
  //  int port;
   // std::string host;
	std::string listen; //for network version 1
	std::vector<std::string> serverNames;
    std::map<std::string, std::string> errorPages;
    size_t clientMaxBodySize;
    std::string root;
    std::string index;
    std::vector<LocationConfig> locations;
    CGIConfig cgi;
	int listen_socket; //why ?
	struct sockaddr_in address;	//why ?
};

#endif
