/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artclave <artclave@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/05 16:31:54 by artclave          #+#    #+#             */
/*   Updated: 2024/09/05 17:44:33 by artclave         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void	check_non_blocking(int sockfd){
	 int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1) {
        std::cerr<<"fcntl";
        exit(EXIT_FAILURE);
    }
    if (flags & O_NONBLOCK) {
        std::cout<<"Socket is non-blocking.\n";
    } else {
        std::cout<<"Socket is blocking.\n";
    }

}

void	get_response(HttpRequest &http){
	
}

void	server(ServerConfig const &config){
	int fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	check_non_blocking(fd);
	struct sockaddr_in address;
	memset((char *)&address, 0, sizeof(address)); 
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = htonl(INADDR_ANY); 
	address.sin_port = htons(config.getListen()); 
	if (bind(fd, (struct sockaddr *)&address, sizeof(address)) == -1)
		std::cerr<<"Bind failed: "<<strerror(errno)<<"\n";
	if (listen(fd, 32) == -1)
		std::cerr<<"listen failed: "<<strerror(errno)<<"\n";
	//wait for incoming connections
	fd_set read_fds;
	FD_ZERO(&read_fds); //initializes read_fds
	FD_SET(fd, &read_fds); //adds file descriptor
	while (true)
	{	
		if (select(fd + 1, &read_fds, NULL, NULL, NULL) <= 0) //starts monitoring fds
			continue ;
		struct sockaddr_in client_addr;
		socklen_t client_len = sizeof(client_addr);
		if (FD_ISSET(fd, &read_fds) == 0) //here we are checking if the current fd is part of the list (meaning its not ready for reading, if so continue)
			continue;
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
		std::cout<<"RESULT: "<<get_response(request)<<"\n\n";
		
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
}