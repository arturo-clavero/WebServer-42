/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSocket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artclave <artclave@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/30 21:35:37 by artclave          #+#    #+#             */
/*   Updated: 2024/10/03 20:36:48 by artclave         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENTSOCKET_HPP
#define CLIENTSOCKET_HPP

#include "includes.hpp"
#include "includes.hpp"
#include "config/ConfigParser.hpp"
#include "config/ServerConfig.hpp"
#include "request_parser/HttpRequest.hpp"
#include "request_parser/RequestParser.hpp"
#include "response_builder/ResponseBuilder.hpp"
#include "Multiplex.hpp"

class ServerCore;
class ServerSocket;

class	ClientSocket{
	private:
		Multiplex	*multiplex;
				int				fd;
		int				state;
		int				read_operations, write_operations;

		std::string		read_buffer;
		ServerConfig	match_config;
		int				file_fd;
		RequestResponse response;
		HttpRequest		request;

		std::string		write_buffer;
		
		int				write_offset;
		pid_t			cgi_pid;
		clock_t			start_time;
		bool			timeout;
		int				pipe_fd[2];
		bool			no_perm;
		
		void	read_request();
		void	init_http_process(std::vector<ServerConfig> &possible_configs);
		void	find_match_config(std::vector<ServerConfig> &possible_configs, const std::string host);
		void	execute_cgi();
		void	wait_cgi();
		void	correct_cgi();
		void	incorrect_cgi();
		void	manage_files();
		void	write_response();
		
	public:
		ClientSocket(Multiplex *server_multiplex, int fd_);
		~ClientSocket();
		void	process_connection(ServerSocket &socket);
		int	get_fd() const;
		int	get_state() const;
};



#endif