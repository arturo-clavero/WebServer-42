/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bperez-a <bperez-a@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/05 16:31:54 by artclave          #+#    #+#             */
/*   Updated: 2024/09/25 12:26:41 by bperez-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <string.h>

int	extract_port(const std::string &str){
	std::string::const_iterator semi_colon = std::find(str.begin(), str.end(), ':');
	int pos = std::distance(str.begin(), semi_colon);
	return (std::atoi(str.substr(pos + 1).c_str()));
}

uint32_t	extract_host(const std::string &str){
	std::string::const_iterator it[5];
	int	oct[4];
	it[0] = str.begin();
	for (int i = 1; i < 4; i++)
		it[i] = std::find(it[i - 1] + 1, str.end(), '.') + 1;
	it[4] = std::find(str.begin(), str.end(), ':') + 1;
	for (int i = 0; i < 4; i++)
	{
		std::string substr = str.substr(it[i] - str.begin(), it[i + 1] - it[i] - 1);
		oct[i] = static_cast<uint8_t>(std::atoi(substr.c_str()));
	}
	return (oct[0] << 24 | oct[1] << 16 | oct[2] << 8 | oct[3]);
}

Server::Server(std::vector<ServerConfig>& config) : config(config) {
	//1. we map each unique "host-port" combo to a vector of all the configs that match that combo.
	std::map<std::string, std::vector<ServerConfig>	> combos;
	for (int i = 0; i < static_cast<int>(config.size()); i++)
		combos[config[i].getListen()].push_back(config[i]);
	//2.Translate map to vector of servers: (used map at first because it makes matching easier than with vector.... Now use vector instead of map for more efficient iteration in loop server)
	 //we use this specific address structure for info about ipv4, then we will cast it to general structure sockaddr which is accepted by networking fts
	std::vector<struct serverSocket>	new_server(combos.size());
	int i = 0;
	for (std::map<std::string, std::vector<ServerConfig> >::iterator it = combos.begin(); it != combos.end(); it++)
	{
		new_server[i].port = extract_port(it->first);
		new_server[i].host = extract_host(it->first);
		// std::cout<<"test server constructor\n"<<"port '"<<new_server[i].port<<"'\nhost '"<<new_server[i].host<<"'\n";
		new_server[i].possible_configs = it->second;
		serverList.push_back(new_server[i]);//add server struct to vector of servers...
		i++;
	}
	// std::cout<<"serverlist size() is "<<serverList.size()<<"\n\n";
	timeout.tv_sec = 2;       // 2 seconds
    timeout.tv_usec = 500000; // 500 milliseconds
	signal(SIGPIPE, SIG_IGN);
}

Server::~Server(){}

int	Server::server_socket_error(std::string type, int *i){
	std::cerr<<type<<" failed: "<<std::strerror(errno)<<"\n";
	std::cerr<<"For server with host and port: "<<serverList[*i].possible_configs[0].getListen()<<"\n";
	close(serverList[*i].fd);
	serverList.erase(serverList.begin() + *i);
	*i = *i - 1;
	return (2);
}

int	Server::server_sockets_for_listening(){
	// std::cout<<"server_sockets_for_listening()\nserverlist size-> "<<serverList.size()<<"\n";
	int opt = 1;
	struct sockaddr_in address_ipv4;
	for (int i = 0; i < static_cast<int>(serverList.size()); i++)
	{
		serverList[i].fd = socket(AF_INET, SOCK_STREAM | O_NONBLOCK, 0); //created socket fd, NON-BLOCKING flag
		if (serverList[i].fd == -1)
			return (server_socket_error("Socket", &i));
		setsockopt(serverList[i].fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));//We can bind to an address that is already bound. Without SO_REUSEADDR, server can fail to bind if the socket it is still held by the prev server run. 
		memset(&address_ipv4, 0, sizeof(address_ipv4));
		address_ipv4.sin_family = AF_INET;//family AF_INET for ipv4
		address_ipv4.sin_port = htons(serverList[i].port); //converts port from host byte order to network byte order
		address_ipv4.sin_addr.s_addr = htonl(serverList[i].host); //converts ip int from host byte order to network byte order
		if (bind(serverList[i].fd, (struct sockaddr *)&address_ipv4, sizeof(address_ipv4)) == -1) //binding socket to address of the port we are supposed to listen from
			return (server_socket_error("Bind", &i));
		if (listen(serverList[i].fd, 32) == -1) //listen to max 32 users
			return (server_socket_error("Listen", &i));
		monitor_fds.push_back(serverList[i].fd);
	}
	// std::cout<<"testing server sockets for listening\n";
	return 0;
}

void	Server::init_client_struct(struct clientSocket &client){
	client.read_buffer.clear();
	client.write_buffer.clear();
	client.write_offset = 0;
	client.read_done = false;
	client.http_done = false;
	client.file_done = false;
}

void	Server::accept_new_client_connection(struct serverSocket &server){
//	std::cout<<"new client connection attempted\n";
	client.fd = accept(server.fd, server.address_ptr, &server.address_len);//accept connections! (Now the client can connect) can only use flags with accept4 which is not allowed in subject //this client fd is a duplicate of our listenning fd but we will use for reading/writing because other fd is listening.... 
	if (client.fd < 0) //no new connections ....
		return ;
	std::cout<<"\n\nnew client connection accepted\n\n";
	setsockopt(client.fd, SOL_SOCKET, SO_RCVTIMEO, (void *)&timeout, sizeof(timeout));
	int enable = 1;
	setsockopt(client.fd, SOL_SOCKET, SO_KEEPALIVE, &enable, sizeof(enable));
	int idle_time = 3; // 30 seconds
    setsockopt(client.fd, IPPROTO_TCP, TCP_KEEPIDLE, &idle_time, sizeof(idle_time));
	init_client_struct(client);
	monitor_fds.push_back(client.fd);
	server.clientList.push_back(client);
}


void	Server::close_connection(struct clientSocket &client, struct serverSocket &server, int j)
{
	std::cout<<"\n\nCLOSING CONNECTION\n\n";
	monitor_fds.remove(client.fd);
	close(client.fd);
	server.clientList.erase(server.clientList.begin() + j);
}

void	Server::write_response(struct clientSocket &client, struct serverSocket &server, int j)
{
	if (client.file_done == false || client.write_operations > 0 || !FD_ISSET(client.fd, &write_set))
		return ;
	setsockopt(client.fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
	int bytes = send(client.fd, &client.write_buffer[client.write_offset], sizeof(client.write_buffer), 0);
	if (bytes < 0)
	{
		 close_connection(client, server, j);
		 return ;
	}
	client.write_operations++;
	client.write_offset += bytes;
	if (client.write_offset >= static_cast<int>(client.write_buffer.size()))
	{
		close_connection(client, server, j);
		return ;
	}
}

void	Server::manage_files(struct clientSocket &client, struct serverSocket &server, int j)
{
	(void)server;
	(void)j;
	if (client.http_done == false || client.file_done == true)
		return ;
	if (!client.response.getFilePathForBody().empty() && client.file_fd > 0)
	{
		if (client.read_operations > 0)
			return ;
		int body_done = client.response.buildBodyFromFile(client.match_config, client.file_fd);
		client.read_operations ++;
		if (body_done == false)
			return ;
	}
	if (!client.response.getPostFileContents().empty() && !client.response.getPostFileFds().empty())
	{
		if (client.write_operations > 0)
			return ;
		int bytes = write(client.response.getPostFileFds().back(), &(client.response.getPostFileContents().back())[client.write_offset], WRITE_BUFFER_SIZE);
		if (bytes < 0)
			return ; //some error saving teh file what to do here?
		client.write_operations++;
		client.write_offset += bytes;
		if (client.write_offset < static_cast<int>(client.response.getPostFileContents().back().size()))
			return ;
		client.write_offset = 0;
		close(client.response.getPostFileFds().back());
		client.response.popBackPostFileFds();
		client.response.popBackPostFileContents();
		if (!client.response.getPostFileContents().empty() && !client.response.getPostFileFds().empty())
			return ;
	}
	client.write_buffer = client.response.toString();
	client.file_done = true;
}

void	Server::find_match_config(struct clientSocket &client, std::vector<ServerConfig> &possible_configs, const std::string host)
{
	std::vector<std::string> possible_names;
	client.match_config = possible_configs[0];
	for (int i = 0; i < static_cast<int>(possible_configs.size()); i++)
	{
		possible_names = possible_configs[i].getServerNames();
		for (int j = 0; j < static_cast<int>(possible_names.size()); j++)
		{
			if (possible_names[j] == host)
			{
				client.match_config = possible_configs[i];
				return ;
			}
		}
	}
}


void	Server::init_http_process(struct clientSocket &client, struct serverSocket &server)
{
	if (client.read_done == false || client.http_done == true)
		return ;
	HttpRequest request = RequestParser::parse(client.read_buffer);
	find_match_config(client, server.possible_configs, request.getHost());
	client.response = ResponseBuilder::build(request, client.match_config);
	std::cout << "DEBUG: Response built" << std::endl;
	// OPEN GET FILES
	if (!client.response.getFilePathForBody().empty())
	{
		client.file_fd = open(client.response.getFilePathForBody().c_str(), O_RDONLY);
	}	
	client.http_done = true;
}

void	Server::read_request(struct clientSocket &client, struct serverSocket &server, int j)
{
	if (client.read_done == true || !FD_ISSET(client.fd, &read_set))
		return ;
	std::string	buff(READ_BUFFER_SIZE, 0);
	std::size_t	pos_zero, pos_content_length, pos_header_end;
	int bytes = recv(client.fd, &buff[0], READ_BUFFER_SIZE, 0);
	if (bytes <= 0)
	{
		close_connection(client, server, j);
		return ;
	}
	client.read_operations++;
	for (int i = 0; i < bytes; i++)
		client.read_buffer += buff[i];
	pos_header_end = client.read_buffer.find("\r\n\r\n");
	if (pos_header_end == std::string::npos) //means header is incomplete because header ends with \r\n\r\h;
	{
		setsockopt(client.fd, SOL_SOCKET, SO_RCVTIMEO, (void *)&timeout, sizeof(timeout));
		return ;
	}
	pos_content_length = client.read_buffer.find("Content-Length:");
	if (pos_content_length != std::string::npos)
	{
		int expected_body_size = std::atoi(client.read_buffer.substr(pos_content_length + 16, pos_header_end).c_str());
		if (static_cast<int>(client.read_buffer.size() - pos_header_end) < expected_body_size)
		{
			setsockopt(client.fd, SOL_SOCKET, SO_RCVTIMEO, (void *)&timeout, sizeof(timeout));
			return ; //incomplete body type 1!	
		}
	}
	else if (client.read_buffer.find("Transfer-Encoding: chunked") != std::string::npos)
	{
		pos_zero = client.read_buffer.find("0\r\n\r\n");
		if (pos_zero == std::string::npos || client.read_buffer[pos_zero + 5] != 0)
		{
			setsockopt(client.fd, SOL_SOCKET, SO_RCVTIMEO, (void *)&timeout, sizeof(timeout));
			return ; //incomplete body type 2!
		}
	}
	client.read_done = true;
}

void	Server::process_client_connection(struct clientSocket &client, struct serverSocket &server, int j)
{
	client.read_operations = 0;
	client.write_operations = 0;
	std::string buff(READ_BUFFER_SIZE, 0);
	read_request(client, server, j);
	init_http_process(client, server);
	manage_files(client, server, j);
	write_response(client, server, j);
}

void	Server::init_sets_for_select(){
	FD_ZERO(&read_set);
	FD_ZERO(&write_set);
	for (std::list<int>::iterator it = monitor_fds.begin(); it != monitor_fds.end(); it++)
	{
		FD_SET(*it, &read_set);
		FD_SET(*it, &write_set);
	}
}

void	Server::run(){
	server_sockets_for_listening();
	while (serverList.size() > 0)
	{
		init_sets_for_select();
		last_socket = monitor_fds.back() + 1;
		if (select(last_socket + 1, &read_set, &write_set, 0, 0) <= 0)
		{
			std::cerr<<"select  failed -> "<<strerror(errno)<<"\n";
		}
		for (int i = 0; i < static_cast<int>(serverList.size()); i++)
		{
			for (int j = 0; j < static_cast<int>(serverList[i].clientList.size()); j++)
			{
				process_client_connection(serverList[i].clientList[j], serverList[i], j);
			}
			if (FD_ISSET(serverList[i].fd, &read_set))
			{
				accept_new_client_connection(serverList[i]);
			}
		}
	}
}