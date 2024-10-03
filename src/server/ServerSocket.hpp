/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSocket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artclave <artclave@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/01 13:20:53 by artclave          #+#    #+#             */
/*   Updated: 2024/10/04 04:20:32 by artclave         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERSOCKET_HPP
# define SERVERSOCKET_HPP

#include "includes.hpp"
#include "ClientSocket.hpp"

class ServerSocket
{
	private:
	private:
		int				fd;
		uint32_t		host;
		int				port;
		Configs	possible_configs;
		Clients	clientList;
		Multiplex					*multiplex;

	public:
		ServerSocket(HostPortConfigMap::iterator it, Multiplex *core_multiplex);
		ServerSocket();
		~ServerSocket();
		void	start_listening();
		void	accept_new_client_connection();
		void	delete_disconnected_clients();
		Clients	&getClients();
		Configs	&get_possible_configs();
};

typedef std::vector<ServerSocket> Servers;

#endif