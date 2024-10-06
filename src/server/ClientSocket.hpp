/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSocket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artclave <artclave@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/30 21:35:37 by artclave          #+#    #+#             */
/*   Updated: 2024/10/07 05:08:48 by artclave         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENTSOCKET_HPP
#define CLIENTSOCKET_HPP

#include "../includes.hpp"
#include "../config/ConfigParser.hpp"
#include "../config/ServerConfig.hpp"
#include "../request_parser/HttpRequest.hpp"
#include "../request_parser/RequestParser.hpp"
#include "../response_builder/ResponseBuilder.hpp"
#include "Multiplex.hpp"
#include "Cgi.hpp"

class ServerCore;
class ServerSocket;

class	ClientSocket{
	friend class Cgi;
	
	private:
		Multiplex		*multiplex;
		int				fd;
		int				state;
		int				read_operations, write_operations;
		
		ServerConfig	match_config;
		RequestResponse response;
		HttpRequest		request;

		std::string		read_buffer;
		int				file_fd;
		std::string		write_buffer;
		int				write_offset;
		
		Cgi				cgi;
		//File			file;
		
		void	read_request();
		void	init_http_process(Configs &possible_configs);
		void	manage_files();
		void	write_response();
		
	public:
		ClientSocket(Multiplex *server_multiplex, int fd_);
		~ClientSocket();
		void	process_connection(ServerSocket &socket);
		int	get_fd() const;
		int	get_state() const;
};

typedef std::vector<ClientSocket> Clients;

#endif