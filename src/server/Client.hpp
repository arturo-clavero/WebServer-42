/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artclave <artclave@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/30 21:35:37 by artclave          #+#    #+#             */
/*   Updated: 2024/09/30 21:53:33 by artclave         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "includes.hpp"
#include "includes.hpp"
#include "config/ConfigParser.hpp"
#include "config/ServerConfig.hpp"
#include "request_parser/HttpRequest.hpp"
#include "request_parser/RequestParser.hpp"
#include "response_builder/ResponseBuilder.hpp"

class Server;

class	Client{
	public:
		int				fd;
		ServerConfig	match_config; //we match a config based on request *server_name
		std::string		read_buffer, write_buffer; //buffer for receive and send, will be initialized to null
		int				write_offset; //offset to see how much we have read or written (for incomplete operations)
		int				state;
		int				file_fd;
		RequestResponse response;
		HttpRequest		request;
		int				read_operations, write_operations; //will be initialzed to 0 for every client loop, to track that we are not exceeding one operation
		pid_t			cgi_pid;
		Client();
		~Client();
		void	init_client_struct();

};



#endif