/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artclave <artclave@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/05 16:31:54 by artclave          #+#    #+#             */
/*   Updated: 2024/09/07 20:19:32 by artclave         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
       #include <string.h>

Server::Server(std::vector<ServerConfig>& config) : config(config) {
	(void)config;
}

Server::~Server(){
	
}

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
//    std::vector<ServerConfig>& config;

std::vector<int> Server::create_listen_sockets(struct socaddr &addr){
	std::vector<int> result;
	// const int i = 0;
	for (std::vector<ServerConfig>::iterator it = config.begin(); it != config.end(); it++){
		int socket_fd = socket(AF_INET, SOCK_STREAM | O_NONBLOCK, 0); //created socket fd
	//	check_non_blocking(fd); //we can delete later
		//memset(addr, 0, sizeof(addr)); //initialzie structure
		
	//	addr.sin_addr.s_addr = config[0].getHost();//so to listen to EVERY ip address, test withput this later
		// addr.sa_port = config[0].getPort(); //adding the port of current server file
	//	std::cout<<"CHecking "<<i<<"\n";
		if (bind(socket_fd, &addr, sizeof(addr)) == -1) //binding socket to address of the port we are supposed to listen from
			std::cerr<<"Bind failed: "<<std::strerror(errno)<<"\n";
		if (listen(socket_fd, 32) == -1) //listen to max 32 users
			std::cerr<<"Listen failed: "<<std::strerror(errno)<<"\n";
		result.push_back(socket_fd);
	}
	return result;
}
void Server::run(){
		//1. Creates, binds and listens -> create listening socket ?
// Create a Socket: Use socket() to create a socket for communication.
// Bind: Use bind() to bind the socket to an address and port.
// Listen: Use listen() to listen for incoming connections.

	struct sockaddr addr; //use sockaddr_in, instead of sockaddr for ipv4
	addr.sa_family = AF_INET; //domain family AF_INET for ipv4 address, same as socket
	std::vector<int> socket_fds = create_listen_sockets(addr);
	fd_set read_fd, write_fd, master;
	FD_ZERO(&master);
	for (std::vector<int>::iterator it =socket_fds.begin(); it != socket_fds.end(); it++)
		FD_SET(*it, &master);
	while (1){
		read_fd = master;
		write_fd = master;
		select(static_cast<int>(socket_fds.size()) + 1, &read_fd, &write_fd, 0, 0);
		for (std::vector<int>::iterator s_fd = socket_fds.begin(); it != socket_fds.end(); it++)
		{
			if (FD_ISSET(*s_fd, read_fd))
				client_fd = accept(*s_fd, &addr, &addr.size(), O_NONBLOCK);
		}
		for (std::vector<int>::iterator s_fd = socket_fds.begin(); it != socket_fds.end(); it++)
		{
			if (FD_ISSET(*s_fd, write_fd))
				client_fd = accept(*s_fd, &addr, &addr.size(), O_NONBLOCK);
		}
		//accept

	}
		//2.Set up select

		//3. Accept connection 
// Accept: Use accept() to accept incoming connections.
// Respond: Send a simple HTTP response using send().
// Close: Close the connection using close().
}