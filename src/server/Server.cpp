/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bperez-a <bperez-a@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/05 16:31:54 by artclave          #+#    #+#             */
/*   Updated: 2024/09/09 14:06:05 by bperez-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <string.h>
#include "ServerSocket.hpp"

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

// std::vector<int> Server::create_listen_sockets(struct socaddr &addr){
// 	std::vector<int> result;
// 	// const int i = 0;
// 	for (std::vector<ServerConfig>::iterator it = config.begin(); it != config.end(); it++){
// 		int socket_fd = socket(AF_INET, SOCK_STREAM | O_NONBLOCK, 0); //created socket fd
// 	//	check_non_blocking(fd); //we can delete later
// 		//memset(addr, 0, sizeof(addr)); //initialzie structure
		
// 	//	addr.sin_addr.s_addr = config[0].getHost();//so to listen to EVERY ip address, test withput this later
// 		// addr.sa_port = config[0].getPort(); //adding the port of current server file
// 	//	std::cout<<"CHecking "<<i<<"\n";
// 		if (bind(socket_fd, &addr, sizeof(addr)) == -1) //binding socket to address of the port we are supposed to listen from
// 			std::cerr<<"Bind failed: "<<std::strerror(errno)<<"\n";
// 		if (listen(socket_fd, 32) == -1) //listen to max 32 users
// 			std::cerr<<"Listen failed: "<<std::strerror(errno)<<"\n";
// 		result.push_back(socket_fd);
// 	}
// 	return result;
// }
void Server::createUniquePair() //create unique pairs of host (IP address) and port from a collection of server_configs
{
	std::cout << "Creating unique pairs of host and port..." << std::endl;
	for (size_t i = 0; i < config.size(); i++) {
		std::cout << "Host: " << config[i].getHost() << " Port: " << config[i].getPort() << std::endl;
		std::pair<std::string, int> uniquePair = std::make_pair(config[i].getHost(), config[i].getPort()); //auto type /pair from different type;
		std::cout << "Adding pair: " << uniquePair.first << ":" << uniquePair.second << std::endl;
		unique.insert(uniquePair);
	}
	std::cout << "Total unique pairs: " << unique.size() << std::endl;
}

void Server::createEventManager() {
	std::cout << "Creating event manager..." << std::endl;
	for(std::set<std::pair<std::string, int> >::iterator it = unique.begin(); it != unique.end(); it++){
		std::cout << "Initializing listen socket for " << it->first << ":" << it->second << std::endl;
		ServerSocket listenSocket;
		listenSocket.initListenSocket(it->first, it->second);
		std::cout << "Listen socket initialized with fd: " << listenSocket.getListenSocket() << std::endl;
	}
	std::cout << "Event manager creation completed." << std::endl;
}
void Server::run(){
	createUniquePair(); //prevent duplicate configurations, prepares the data for the createEventManager()
	createEventManager();// uses only unique pairs to set up listening sockets
// 		//1. Creates, binds and listens -> create listening socket ?
// // Create a Socket: Use socket() to create a socket for communication.
// // Bind: Use bind() to bind the socket to an address and port.
// // Listen: Use listen() to listen for incoming connections.

// 		//accept

// 	}
// 		//2.Set up select

// 		//3. Accept connection 
// // Accept: Use accept() to accept incoming connections.
// // Respond: Send a simple HTTP response using send().
// // Close: Close the connection using close().
}