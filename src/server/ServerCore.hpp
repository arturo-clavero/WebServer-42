/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerCore.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artclave <artclave@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/05 16:33:26 by artclave          #+#    #+#             */
/*   Updated: 2024/10/01 15:18:39 by artclave         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCORE_HPP
#define SERVERCORE_HPP

#include "includes.hpp"
#include "config/ConfigParser.hpp"
#include "config/ServerConfig.hpp"
#include "request_parser/HttpRequest.hpp"
#include "request_parser/RequestParser.hpp"
#include "response_builder/ResponseBuilder.hpp"

#include "Multiplex.hpp"
#include "ServerSocket.hpp"
#include "ClientSocket.hpp"

class ServerCore {
	private:
		std::vector<ServerConfig>	config;
		std::vector<ServerSocket>	serverList;
		Multiplex					multiplex;
		void	set_up_signals();
		std::map<std::string, std::vector<ServerConfig>	> unique_host_port_configs();
		void	set_up_server_sockets(std::map<std::string, std::vector<ServerConfig> > combos);

	public:
		ServerCore(std::vector<ServerConfig>& config);
		void	run();
		~ServerCore();
};

void	signalHandler(int signal) ;
		
#endif