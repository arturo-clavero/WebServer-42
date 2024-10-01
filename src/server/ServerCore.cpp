/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerCore.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artclave <artclave@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/05 16:31:54 by artclave          #+#    #+#             */
/*   Updated: 2024/10/01 15:18:26 by artclave         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerCore.hpp"
#include "ClientSocket.hpp"
#include "ServerSocket.hpp"

#include <string.h>
#include <ctime>

int server_running = true;

ServerCore::ServerCore(std::vector<ServerConfig>& config) : config(config){}
ServerCore::~ServerCore(){}

void	signalHandler(int signal) //UTILS OR CORE ?
{
	(void)signal;
	server_running = false;
}

void	ServerCore::set_up_signals()
{
	std::signal(SIGTERM, signalHandler);
	std::signal(SIGKILL, signalHandler);
	std::signal(SIGPIPE, signalHandler);
	std::signal(SIGINT, signalHandler);//this is for control c not sure if w should override it too....
}

std::map<std::string, std::vector<ServerConfig>	> ServerCore::unique_host_port_configs()
{
	std::map<std::string, std::vector<ServerConfig>	> combos;
	for (int i = 0; i < static_cast<int>(config.size()); i++)
		combos[config[i].getListen()].push_back(config[i]);
	config.clear();
	return combos;
}

void	ServerCore::set_up_server_sockets(std::map<std::string, std::vector<ServerConfig> > combos)
{
	for (std::map<std::string, std::vector<ServerConfig> >::iterator it = combos.begin(); it != combos.end(); it++)
	{
		try
		{
			ServerSocket new_server(it, &multiplex);
			new_server.start_listening();
			serverList.push_back(new_server);
		}
		catch (char *mssg) {std::cerr<<mssg<<"\n";}
	}
	if (static_cast<int>(serverList.size()) == 0)
	{
		std::cerr<<"Sorry there are no valid servers for listening....\n";
		exit(2);
	}
}

void	ServerCore::run(){
	//set_up_signals(); //UNCOMMENT!
	set_up_server_sockets(unique_host_port_configs());
	while (server_running)
	{
		multiplex.reset_select();
		for	(std::vector<ServerSocket>::iterator server_it = serverList.begin(); server_it != serverList.end(); server_it++)
		{
			for (std::vector<ClientSocket>::iterator client_it = server_it->getClients().begin(); client_it != server_it->getClients().end(); client_it++)
			{
				std::cout<<"client "<<client_it->fd<<", state "<<client_it->state<<"\n";
				client_it->process_connection(*server_it);
			}
			server_it->delete_disconnected_clients();
			server_it->accept_new_client_connection();
		}
	}
	server_running = true;
	multiplex.close_all_active_fds();
}