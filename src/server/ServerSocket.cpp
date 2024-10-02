/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artclave <artclave@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/01 13:25:22 by artclave          #+#    #+#             */
/*   Updated: 2024/10/02 20:04:03 by artclave         ###   ########.fr       */
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

void	ServerSocket::start_listening()
{
	int opt = 1;
	int flags;
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1)
		throw(strerror(errno)) ;
	flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
		throw(strerror(errno)) ;
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
		throw(strerror(errno)) ;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	struct sockaddr_in address_ipv4;
	memset(&address_ipv4, 0, sizeof(address_ipv4));
	
	address_ipv4.sin_family = AF_INET;
	address_ipv4.sin_addr.s_addr = htonl(host);
	address_ipv4.sin_port = htons(port);
	address_ptr = (struct sockaddr *)&address_ipv4;
	address_len = sizeof(address_ipv4);
	if (bind(fd, (struct sockaddr *)&address_ipv4, sizeof(address_ipv4)) == -1)
		throw(strerror(errno)) ;
	if (listen(fd, 32) == -1)
		throw(strerror(errno)) ;
	multiplex->add(fd);
}

void	ServerSocket::accept_new_client_connection()
{
	int client_fd;
	if (!multiplex->ready_to_read(fd))
		return ;
	struct sockaddr_in address_ipv4;
	memset(&address_ipv4, 0, sizeof(address_ipv4));
	address_ipv4.sin_family = AF_INET;
	address_ipv4.sin_port = htons(port);
	address_ipv4.sin_addr.s_addr = htonl(host);

	address_len = sizeof(address_ipv4);
	client_fd = accept(fd, (struct sockaddr *)&address_ipv4, &address_len);
	if (client_fd < 0)
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
