/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artclave <artclave@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/05 16:33:26 by artclave          #+#    #+#             */
/*   Updated: 2024/09/30 21:53:00 by artclave         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "includes.hpp"
#include "config/ConfigParser.hpp"
#include "config/ServerConfig.hpp"
#include "request_parser/HttpRequest.hpp"
#include "request_parser/RequestParser.hpp"
#include "response_builder/ResponseBuilder.hpp"
#include "Client.hpp"
//class Client;

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

struct	serverSocket{
	int				fd;
	uint32_t		host;
	int				port;
	struct sockaddr *address_ptr;
	socklen_t		address_len;
	std::vector<ServerConfig>	possible_configs;
	std::vector<Client>	clientList;
};

class Server {
	private:
		std::vector<ServerConfig>	config;
		std::vector<serverSocket>	serverList;
		fd_set						read_set, write_set;
		Client	client;
		struct timeval timeout;
		std::list<int>	monitor_fds;
		
		int		server_sockets_for_listening();
		int		server_socket_error(std::string type, int *i);
		void	accept_new_client_connection(struct serverSocket &server);
		void	close_connection(Client &client, struct serverSocket &server, int j);
		void	process_client_connection(Client &client, struct serverSocket &socket);
		void	delete_disconnected_clients(struct serverSocket &server);
		void	read_request(Client &client);
		void	find_match_config(Client &client, std::vector<ServerConfig> &possible_configs, const std::string host);
		void	execute_cgi(Client &client);
		void	wait_cgi(Client &client);
		void	manage_files(Client &client);
		void	write_response(Client &client);
		void	init_http_process(Client &client, struct serverSocket &server);
		void	init_sets_for_select();

		

	public:
		Server(std::vector<ServerConfig>& config);
		void	run();
		~Server();
};

#endif