/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSocket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artclave <artclave@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/30 21:35:37 by artclave          #+#    #+#             */
/*   Updated: 2024/10/01 22:04:14 by artclave         ###   ########.fr       */
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


enum connection_states
{
	READING,
	HTTP,
	EXECUTECGI,
	WAITCGI,
	FILES,
	WRITE,
	DISCONNECT,
};

class ServerCore;
class ServerSocket;

class	ClientSocket{
	private:
		Multiplex	*multiplex;
				int				fd;//**SHARED ACROSS ALL
		int				state; //**SHARED ACROSS ALL
		int				read_operations, write_operations; //**SHARED ACROSS ALL

		std::string		read_buffer; //[STEP 1, 2]
		ServerConfig	match_config; // [STEP 2, 3ALL] init http process, execute_cgi, wait cgi, manage files
		int				file_fd;//STEP 2 AND STEP 3 but could be just step 3 I guess ....
		RequestResponse response;//step 2 and 3
		HttpRequest		request;//step 2 and 3

		std::string		write_buffer; //[STEP end 3ALL, 4]
		
		int				write_offset; //IGN [INDEPENDENT VALUE IN 3ALL AND 4]
		pid_t			cgi_pid; // IGN [INSIDE WAIT]
		
		void	read_request(); //STATES
		void	init_http_process(std::vector<ServerConfig> &possible_configs);//STATES
		void	find_match_config(std::vector<ServerConfig> &possible_configs, const std::string host);
		void	execute_cgi();//STATES
		void	wait_cgi();//STATES
		void	manage_files();//STATES
		void	write_response();//STATES
		
	public:
		ClientSocket(Multiplex *server_multiplex, int fd_);
		~ClientSocket();
		void	process_connection(ServerSocket &socket);//CLIENT
		int	get_fd() const;
		int	get_state() const;
		
		
		
		

	//	void	init_client_struct();

};



#endif