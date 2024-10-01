/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artclave <artclave@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/01 13:25:22 by artclave          #+#    #+#             */
/*   Updated: 2024/10/01 22:06:17 by artclave         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerSocket.hpp"
#include "Utils.hpp"

ServerSocket::ServerSocket(std::map<std::string, std::vector<ServerConfig> >::iterator it, Multiplex *core_multiplex)
	:
	host(Utils::extract_host(it->first)),
	port(Utils::extract_port(it->first)),
	possible_configs(it->second), 
	multiplex(core_multiplex)
	{}
	
ServerSocket::ServerSocket(){}
ServerSocket::~ServerSocket(){}

void	ServerSocket::set_address()
{
	struct sockaddr_in address_ipv4;
	memset(&address_ipv4, 0, sizeof(address_ipv4));
	address_ipv4.sin_family = AF_INET;//family AF_INET for ipv4
	address_ipv4.sin_port = htons(port); //converts port from host byte order to network byte order
	address_ipv4.sin_addr.s_addr = htonl(host); //converts ip int from host byte order to network byte order
	address_ptr = (struct sockaddr *)&address_ipv4;
	address_len = sizeof(address_ipv4);
}

void	ServerSocket::start_listening()
{
	int opt = 1;
	int flags;
	fd = socket(AF_INET, SOCK_STREAM, 0); //created socket fd, NON-BLOCKING flag
	if (fd == -1)
		throw(strerror(errno)) ;
	flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
		throw(strerror(errno)) ;
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
		throw(strerror(errno)) ;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));//We can bind to an address that is already bound. Without SO_REUSEADDR, server can fail to bind if the socket it is still held by the prev server run. 
	this->set_address();
	if (bind(fd, address_ptr, address_len) == -1) //binding socket to address of the port we are supposed to listen from
		throw(strerror(errno)) ;
	if (listen(fd, 32) == -1) //listen to max 32 users
		throw(strerror(errno)) ;
	multiplex->add(fd);
}

void	ServerSocket::accept_new_client_connection()
{
	int client_fd;
	if (!multiplex->ready_to_read(fd))
		return ;
	client_fd = accept(fd, address_ptr, &address_len);//accept connections! (Now the client can connect) can only use flags with accept4 which is not allowed in subject //this client fd is a duplicate of our listenning fd but we will use for reading/writing because other fd is listening.... 
	if (client_fd < 0) //no new connections ....
		return ;
	multiplex->add(client_fd);
	clientList.push_back(ClientSocket(multiplex, client_fd));
}

void	ServerSocket::delete_disconnected_clients()
{
	for (int j = 0; j < static_cast<int>(clientList.size()); )
	{
		if (clientList[j].get_state() == DISCONNECT)
		{
			std::cout<<"Disconnected "<<clientList[j].get_fd()<<" \n";
			multiplex->remove(clientList[j].get_fd());
			close(clientList[j].get_fd());
			clientList.erase(clientList.begin() + j);
		}
		else
			j++;
	}
}

std::vector<ClientSocket>	&ServerSocket::getClients() {return clientList; }
std::vector<ServerConfig>	&ServerSocket::get_possible_configs() {return possible_configs; }
