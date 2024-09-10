/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artclave <artclave@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/05 16:31:54 by artclave          #+#    #+#             */
/*   Updated: 2024/09/10 12:02:49 by artclave         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(std::vector<ServerConfig>& config) : config(config) {
	
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

int	Server::create_listen_socket(ServerConfig &config){
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
	address.sin_port = htons(config.getPort()); 
	if (bind(fd, (struct sockaddr *)&address, sizeof(address)) == -1)
		std::cerr<<"Bind failed: "<<strerror(errno)<<"\n";
	if (listen(fd, 32) == -1)
		std::cerr<<"listen failed: "<<strerror(errno)<<"\n";
	//wait for incoming connections
	config.setListenSocket(fd);
	config.setAddress(address);
	return fd;
}

void	Server::run(){
	fd_set read_fds, write_fds;
	int last_fd;
	for (size_t i = 0; i < config.size(); i++)
	{
		last_fd = create_listen_socket(config[i]);
	}
	
	while (true)
	{	
		
	FD_ZERO(&read_fds);
	FD_ZERO(&write_fds);
	for (size_t i = 0; i < config.size(); i++)
	{
		FD_SET(config[i].getListenSocket(), &read_fds); //adds file descriptor
		FD_SET(config[i].getListenSocket(), &write_fds); //adds file descriptor
	}
	if (select(last_fd + 1, &read_fds, &write_fds, NULL, NULL) < 0 )
		std::cerr << "select failed: " << strerror(errno) << std::endl;
		//std::cout << "-------last fd is " << last_fd << std::endl;
	for (size_t i = 0; i < config.size(); i++){
		
		if (FD_ISSET(config[i].getListenSocket(), &read_fds) == 0) //here we are checking if the current fd is part of the list (meaning its not ready for reading, if so continue)
			continue;
		struct sockaddr_in client_addr;
		socklen_t client_len = sizeof(client_addr);
		
		//FD_SET(config[i].getListenSocket(), &read_fds); //adds file descriptor
		int new_socket = accept(config[i].getListenSocket(), (struct sockaddr *)&client_addr, &client_len);
		if (new_socket == -1) {
			std::cerr << "Accept failed: " << strerror(errno) << std::endl;
			continue;
		}
		std::cout << "New connection, socket fd is " << new_socket << std::endl;
		char buffer[30000] = {0};
        recv(new_socket, buffer, 30000,	0);

		
		std::cout << "------------------Request received-------------------" << std::endl;
        std::cout << buffer << std::endl;
		HttpRequest request = RequestParser::parse(buffer);
		std::cout << "------------------Request parsed-------------------" << std::endl;
		std::cout << request << std::endl;
		RequestResponse response = ResponseBuilder::build(request, config[i]);
		std::cout << "------------------Response built-------------------" << std::endl;
		std::cout << response << std::endl;
		//std::cout << response.toString() << std::endl;
		std::string responseStr = response.toString();
		send(new_socket, responseStr.c_str(), responseStr.length(), 0);
        std::cout << "------------------HTML content sent-------------------" << std::endl;
        close(new_socket);
		}
		
	}
	for (size_t i = 0; i < config.size(); i++)
	{
		close(config[i].getListenSocket());
	}
}