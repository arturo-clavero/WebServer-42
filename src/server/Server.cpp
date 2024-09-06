/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bperez-a <bperez-a@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/05 16:31:54 by artclave          #+#    #+#             */
/*   Updated: 2024/09/06 12:26:24 by bperez-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(std::vector<ServerConfig> const &config): config(config){
	
}

Server::~Server(){
	
}

void	Server::check_non_blocking(int sockfd){
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


void	Server::run(){
	int fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	int opt = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "setsockopt failed: " << strerror(errno) << "\n";
        exit(EXIT_FAILURE);
	}
	//check_non_blocking(fd);
	struct sockaddr_in address;
	memset((char *)&address, 0, sizeof(address)); 
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = htonl(INADDR_ANY); 
	address.sin_port = htons(config[0].getListen()); 
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

		
		std::cout << "------------------Request received-------------------" << std::endl;
        std::cout << buffer << std::endl;
		HttpRequest request = RequestParser::parse(buffer);
		std::cout << "------------------Request parsed-------------------" << std::endl;
		std::cout << request << std::endl;
		RequestResponse response = ResponseBuilder::build(request, config[0]);
		std::cout << "------------------Response built-------------------" << std::endl;
		std::cout << response.toString() << std::endl;
		std::string responseStr = response.toString();
		send(new_socket, responseStr.c_str(), responseStr.length(), 0);
        std::cout << "------------------HTML content sent-------------------" << std::endl;
        close(new_socket);
	}
	close(fd);
}