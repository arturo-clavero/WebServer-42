/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artclave <artclave@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 20:09:33 by bperez-a          #+#    #+#             */
/*   Updated: 2024/09/05 16:40:57 by artclave         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "includes.hpp"

#include "server/Server.hpp"
#include "config/ConfigParser.hpp"
#include "config/ServerConfig.hpp"
#include "request_parser/HttpRequest.hpp"
#include "response_builder/ResponseBuilder.hpp"

void printServerConfig(const ServerConfig& config) {
    std::cout << "Server Configuration:" << std::endl;
    std::cout << "Listen: " << config.getListen() << std::endl;
    std::cout << "Host: " << config.getHost() << std::endl;
    std::cout << "Server Name: " << config.getServerName() << std::endl;
    std::cout << "Client Max Body Size: " << config.getClientMaxBodySize() << " bytes" << std::endl;
    std::cout << "Root: " << config.getRoot() << std::endl;
    std::cout << "Index: " << config.getIndex() << std::endl;

    std::cout << "\nError Pages:" << std::endl;
    std::map<int, std::string>::const_iterator it;
    for (it = config.getErrorPages().begin(); it != config.getErrorPages().end(); ++it) {
        std::cout << "  " << it->first << ": " << it->second << std::endl;
    }

    std::cout << "\nLocations:" << std::endl;
    std::vector<LocationConfig>::const_iterator locIt;
    for (locIt = config.getLocations().begin(); locIt != config.getLocations().end(); ++locIt) {
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
    std::cout << "  Root: " << config.getCgi().root << std::endl;
    std::cout << "  Path: " << config.getCgi().path << std::endl;
    std::cout << "  Extension: " << config.getCgi().ext << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <config_file_path>" << std::endl;
        return 1;
    }

    ConfigParser parser(argv[1]);
    ServerConfig config = parser.parse();
	server(config);
    return 0;
}
