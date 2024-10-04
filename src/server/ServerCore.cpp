/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerCore.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artclave <artclave@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/05 16:31:54 by artclave          #+#    #+#             */
/*   Updated: 2024/10/04 07:01:27 by artclave         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerCore.hpp"
#include "ClientSocket.hpp"
#include "ServerSocket.hpp"

#include <string.h>
#include <ctime>

ServerCore::ServerCore(Configs& config) : config(config){}
ServerCore::~ServerCore(){}

HostPortConfigMap ServerCore::unique_host_port_configs()
{
	HostPortConfigMap combos;
	for (int i = 0; i < static_cast<int>(config.size()); i++)
		combos[config[i].getListen()].push_back(config[i]);
	config.clear();
	return combos;
}

void	ServerCore::set_up_server_sockets(HostPortConfigMap combos)
{
	for (HostPortConfigMap::iterator it = combos.begin(); it != combos.end(); it++)
	{
		try
		{
			ServerSocket new_server(it, &multiplex);
			new_server.start_listening();
			serverList.push_back(new_server);
		}
		catch (char *mssg)
		{
			std::cerr<<mssg<<"\n";
		}
	}
	if (static_cast<int>(serverList.size()) == 0)
	{
		std::cerr<<"Sorry there are no valid servers for listening....\n";
		exit(2);
	}
}


void	ServerCore::run(){
	signal(SIGPIPE, SIG_IGN);
	set_up_server_sockets(unique_host_port_configs());
	while (true)
	{
		multiplex.reset_select();
		for	(Servers::iterator server_it = serverList.begin(); server_it != serverList.end(); server_it++)
		{
			for (Clients::iterator client_it = server_it->getClients().begin(); client_it != server_it->getClients().end(); client_it++)
			{
				client_it->process_connection(*server_it);
			}
			server_it->delete_disconnected_clients();
			server_it->accept_new_client_connection();
		}
	}
}