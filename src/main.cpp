/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bperez-a <bperez-a@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 20:09:33 by bperez-a          #+#    #+#             */
/*   Updated: 2024/09/05 15:05:23 by bperez-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config/ConfigParser.hpp"
#include "config/ServerConfig.hpp"
#include "request_parser/HttpRequest.hpp"
#include "includes.hpp"
#include <fstream>
#include <sstream> 

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
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in address;
	memset((char *)&address, 0, sizeof(address)); 
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = htonl(INADDR_ANY); 
	address.sin_port = htons(config.getListen()); 
	//fcntl(fd, F_SETFL, O_NONBLOCK);
	bind(fd, (struct sockaddr *)&address, sizeof(address));
	listen(fd, 32);
	//wait for incoming connections
	fd_set read_fds;
	FD_ZERO(&read_fds);
	FD_SET(fd, &read_fds);
	select(fd + 1, &read_fds, NULL, NULL, NULL);
	while (true)
	{
		struct sockaddr_in client_addr;
		socklen_t client_len = sizeof(client_addr);

		int new_socket = accept(fd, (struct sockaddr *)&client_addr, &client_len);
		if (new_socket == -1) {
			std::cerr << "Accept failed: " << strerror(errno) << std::endl;
			continue;
		}
		std::cout << "New connection, socket fd is " << new_socket << std::endl;
		char buffer[30000] = {0};
        read(new_socket, buffer, 30000);
        std::cout << buffer << std::endl;
		HttpRequest request;
		request.parse(buffer);
		std::cout << request << std::endl;
		

        // Read the content of www/index.html
        std::ifstream file("www/index.html");
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();

        // Convert content length to string using stringstream
        std::stringstream ss;
        ss << content.length();
        std::string contentLengthStr = ss.str();
std::string response;
        // Prepare the HTTP response
		// std::string response;s a test, make it so if request _path is favicon.ico, send a favicon
		if (request.getPath() == "/favicon.ico") {
			std::ifstream favicon("www/images/favicon.ico", std::ios::binary);
			std::string faviconContent((std::istreambuf_iterator<char>(favicon)), std::istreambuf_iterator<char>());
			favicon.close();
			std::stringstream faviconSS;
			faviconSS << faviconContent.length();
			std::string faviconLengthStr = faviconSS.str();
			std::cout << "HELLO FROM FAVICON" << std::endl;
			response = "HTTP/1.1 200 OK\r\nContent-Type: image/x-icon\r\nContent-Length: " + faviconLengthStr + "\r\n\r\n" + faviconContent;
		}
		else {
        	response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + contentLengthStr + "\r\n\r\n" + content;
		}

        // Send the response
        send(new_socket, response.c_str(), response.length(), 0);
        std::cout << "------------------HTML content sent-------------------" << std::endl;
        close(new_socket);
	}
    return 0;
}
