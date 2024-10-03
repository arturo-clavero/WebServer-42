/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSocket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artclave <artclave@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/01 13:20:53 by artclave          #+#    #+#             */
/*   Updated: 2024/10/04 04:05:44 by artclave         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERSOCKET_HPP
# define SERVERSOCKET_HPP

#include "includes.hpp"
#include "ServerCore.hpp"
#include "ClientSocket.hpp"

class ServerSocket
{
	private:
	private:
		int				fd;
		uint32_t		host;
		int				port;
		std::vector<ServerConfig>	possible_configs;
		std::vector<ClientSocket>	clientList;
		Multiplex					*multiplex;

	public:
		ServerSocket(std::map<std::string, std::vector<ServerConfig> >::iterator it, Multiplex *core_multiplex);
		ServerSocket();
		~ServerSocket();
		void	start_listening();
		void	accept_new_client_connection();
		void	delete_disconnected_clients();
		std::vector<ClientSocket>	&getClients();
		std::vector<ServerConfig>	&get_possible_configs();
};

#endif