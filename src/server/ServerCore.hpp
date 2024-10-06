/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerCore.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artclave <artclave@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/05 16:33:26 by artclave          #+#    #+#             */
/*   Updated: 2024/10/07 05:06:52 by artclave         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCORE_HPP
#define SERVERCORE_HPP

#include "../includes.hpp"
#include "../config/ConfigParser.hpp"
#include "../config/ServerConfig.hpp"
#include "../request_parser/HttpRequest.hpp"
#include "../request_parser/RequestParser.hpp"
#include "../response_builder/ResponseBuilder.hpp"

#include "Multiplex.hpp"
#include "ServerSocket.hpp"
#include "ClientSocket.hpp"


class ServerCore {
	private:
		Configs				config;
		Servers				serverList;
		Multiplex			multiplex;
		void				set_up_signals();
		HostPortConfigMap	unique_host_port_configs();
		void				set_up_server_sockets(HostPortConfigMap combos);

	public:
		ServerCore(Configs& config);
		void	run();
		~ServerCore();
};

void	signalHandler(int signal) ;
		
#endif