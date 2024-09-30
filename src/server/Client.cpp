/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artclave <artclave@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/30 21:44:25 by artclave          #+#    #+#             */
/*   Updated: 2024/09/30 22:48:11 by artclave         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Server.hpp"

Client::Client(int fd_) : fd(fd_), write_offset(0), state(0){}

Client::~Client(){}

// void	Server::accept_new_client_connection(struct serverSocket &server){
// 	if (!FD_ISSET(server.fd, &read_set))
// 		return ;
// 	client.fd = accept(server.fd, server.address_ptr, &server.address_len);//accept connections! (Now the client can connect) can only use flags with accept4 which is not allowed in subject //this client fd is a duplicate of our listenning fd but we will use for reading/writing because other fd is listening.... 
// 	if (client.fd < 0) //no new connections ....
// 		return ;
// 	static int p;
// 	std::cout<<"\n\nnew client connection accepted "<<p<<"\n\n";
// 	p++;
// 	/*setsockopt(client.fd, SOL_SOCKET, SO_RCVTIMEO, (void *)&timeout, sizeof(timeout));
// 	int enable = 1;
// 	setsockopt(client.fd, SOL_SOCKET, SO_KEEPALIVE, &enable, sizeof(enable));
// 	int idle_time = 3; // 30 seconds
//     setsockopt(client.fd, IPPROTO_TCP, TCP_KEEPIDLE, &idle_time, sizeof(idle_time));*/
// 	client.init_client_struct();
// 	monitor_fds.push_back(client.fd);
// 	server.clientList.push_back(client);
// }