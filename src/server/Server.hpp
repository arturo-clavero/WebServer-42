/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artclave <artclave@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/05 16:33:26 by artclave          #+#    #+#             */
/*   Updated: 2024/09/12 10:38:49 by artclave         ###   ########.fr       */
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
# define READ_BUFFER_SIZE 300
# define WRITE_BUFFER_SIZE 1024


struct	clientSocket{
	int fd;
	ServerConfig	match_config; //we match a config based on request *server_name
	std::string		read_buffer, write_buffer; //buffer for receive and send, will be initialized to null
	int				write_offset; //offset to see how much we have read or written (for incomplete operations)
	bool			read_complete; //bool to see if reading/writing is done
	bool			wants_to_disconnect, http_done;
	int				read_operations, write_operations; //will be initialzed to 0 for every client loop, to track that we are not exceeding one operation
};

struct	serverSocket{
	int				fd;
	uint32_t		host;
	int				port;
	struct sockaddr *address_ptr;
	socklen_t		address_len;
	std::vector<ServerConfig>	possible_configs;
	std::vector<clientSocket>	clientList;
};

class Server {
	private:
		std::vector<ServerConfig>	config;
		std::vector<serverSocket>	serverList;
		fd_set						read_set, write_set;
		int							last_socket;
		struct clientSocket	client;
		struct timeval timeout;
		std::list<int>	monitor_fds;
		int		server_sockets_for_listening();
		int		server_socket_error(std::string type, int *i);
		void	init_client_struct(struct clientSocket &client);
		void	accept_new_client_connection(struct serverSocket &server);
		void	close_connection(struct clientSocket &client, struct serverSocket &server, int j);
		void	process_client_connection(struct clientSocket &client, struct serverSocket &socket, int j);
		void	read_request(struct clientSocket &client, struct serverSocket &server, int j);
		void	find_match_config(struct clientSocket &client, std::vector<ServerConfig> &possible_configs, const std::string host);
		void	write_response(struct clientSocket &client, struct serverSocket &server, int j);
		void	init_http_process(struct clientSocket &client, struct serverSocket &server);
		void	init_sets_for_select();

		

	public:
		Server(std::vector<ServerConfig>& config);
		void	run();
		~Server();
};

#endif