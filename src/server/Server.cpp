/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bperez-a <bperez-a@student.42bangkok.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/05 16:31:54 by artclave          #+#    #+#             */
/*   Updated: 2024/09/12 22:31:25 by bperez-a         ###   ########.fr       */
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
	client.read_complete = false;
	client.write_offset = 0;
	client.wants_to_disconnect = false;
	client.http_done = false;
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
	if (client.read_complete == false)
		return;
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

void	Server::read_request(struct clientSocket &client, struct serverSocket &server, int j)
{
	std::string	buff(READ_BUFFER_SIZE, 0);
	std::size_t	pos_zero, pos_content_length, pos_header_end;
	
	//std::cout<<"attempt to read...\n";
	int bytes = recv(client.fd, &buff[0], READ_BUFFER_SIZE, 0);
	if (bytes <= 0)
	{
		close_connection(client, server, j);
		return ;
	}
	client.read_operations++;
	for (int i = 0; i < bytes; i++)
		client.read_buffer += buff[i];
//	std::cout<<"REading test\n"<<"'"<<client.read_buffer<<"'\n";
	//check for incomplete read:
	pos_header_end = client.read_buffer.find("\r\n\r\n");
	if (pos_header_end == std::string::npos) //means header is incomplete because header ends with \r\n\r\h;
	{
		setsockopt(client.fd, SOL_SOCKET, SO_RCVTIMEO, (void *)&timeout, sizeof(timeout));
	//	std::cout<<"reading is incomplete (header)\n\n";
		return ;
	}
	//std::cout<<"header is complete\n\n";
	pos_content_length = client.read_buffer.find("Content-Length:");
	//check for body
	if (pos_content_length != std::string::npos)
	{
		int expected_body_size = std::atoi(client.read_buffer.substr(pos_content_length + 16, pos_header_end).c_str());
		//std::cout<<"the actual substr: "<<client.read_buffer.substr(pos_content_length + 16, pos_header_end);
		//std::cout<<"content length, expected body size: "<<expected_body_size<<"\n";
		if (static_cast<int>(client.read_buffer.size() - pos_header_end) < expected_body_size)
		{
			setsockopt(client.fd, SOL_SOCKET, SO_RCVTIMEO, (void *)&timeout, sizeof(timeout));
			//std::cout<<"reading incomplete (content len)\n\n";
			return ; //incomplete body type 1!	
		}
	}
	else if (client.read_buffer.find("Transfer-Encoding: chunked") != std::string::npos)
	{
		pos_zero = client.read_buffer.find("0\r\n\r\n");
		if (pos_zero == std::string::npos || client.read_buffer[pos_zero + 5] != 0)
		{
			setsockopt(client.fd, SOL_SOCKET, SO_RCVTIMEO, (void *)&timeout, sizeof(timeout));
			//std::cout<<"reading incomplete (chunks)\n";
			return ; //incomplete body type 2!
		}
	}
	//std::cout<<"READ BUFFER\n"<<client.read_buffer<<"'..XXXXXXXXXXXXXXXXXXXX..'\n\n";
	client.read_complete = true;
}

void	Server::find_match_config(struct clientSocket &client, std::vector<ServerConfig> &possible_configs, const std::string host)
{
	std::vector<std::string> possible_names;
	//default is the first possible config
	client.match_config = possible_configs[0];
	//let's see if we find a better match ...
	//first lets take the host name from request ...
	for (int i = 0; i < static_cast<int>(possible_configs.size()); i++){
		possible_names = possible_configs[i].getServerNames();
		for (int j = 0; j < static_cast<int>(possible_names.size()); j++)
		{
			// //so at first "exact" match we return
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
	if (client.read_complete == false)
		return;
	//std::cout<<"gonna parse it ....\n ";
	HttpRequest request = RequestParser::parse(client.read_buffer);
	std::cout<<"\nREQUEST:\n"<<request<<"'..XXXXXXXXXXXXXXXXXXXX..'\n\n";
	std::cout<<"\nhost is ... "<<request.getHost()<<"\n\n";
	find_match_config(client, server.possible_configs, request.getHost());
	client.write_buffer = ResponseBuilder::build(request, client.match_config).toString();
	std::cout<<"\nRESPONSE:\n"<<client.write_buffer<<"'..XXXXXXXXXXXXXXXXXXXX..'\n\n";
	client.http_done = true;
}

void	Server::process_client_connection(struct clientSocket &client, struct serverSocket &server, int j)
{
	client.read_operations = 0;
	client.write_operations = 0;
	std::string buff(READ_BUFFER_SIZE, 0);
	//check if we should read
	// std::cout<<"client read complete: "<<client.read_complete<<"\n";
	if (client.read_complete == false && FD_ISSET(client.fd, &read_set))
		read_request(client, server, j);
	// std::cout<<"client read complete: "<<client.read_complete<<"\n";
	//check if we 	should build response
	if (client.http_done == false && client.read_complete == true)
		init_http_process(client, server);
	//check if we should write
	if (client.write_operations == 0 && !client.write_buffer.empty() && FD_ISSET(client.fd, &write_set))
		write_response(client, server, j);
	// std::cout<<"client read complete: "<<client.read_complete<<"\n";
}

void	Server::init_sets_for_select(){
	FD_ZERO(&read_set);
	FD_ZERO(&write_set);
	// std::cout<<"\nmonitor!\n";
	for (std::list<int>::iterator it = monitor_fds.begin(); it != monitor_fds.end(); it++)
	{
		// std::cout<<*it<<" ";
		FD_SET(*it, &read_set);
		FD_SET(*it, &write_set);
	}
	// std::cout<<"\n\n";
}

void	Server::run(){
	server_sockets_for_listening();
	while (serverList.size() > 0) {
		init_sets_for_select();
		last_socket = monitor_fds.back() + 1;
		// std::cout<<"last socket "<<last_socket<<"\n";
		if (select(last_socket + 1, &read_set, &write_set, 0, 0) <= 0)//monitor socket fds to see if they are ready for reading(read_fd)/writing(write_fd)
			std::cerr<<"select  failed -> "<<strerror(errno)<<"\n";
		// std::cout<<"IN server list size: "<<serverList.size()<<"\n";
		for (int i = 0; i < static_cast<int>(serverList.size()); i++) //for every server (or listening socket -> one listening socket per server)
		{
			// std::cout<<"client list size for server["<<i<<"]: "<<serverList[i].clientList.size()<<"\n";
			//lets process client conections we made before with this server socket
			for (int j = 0; j < static_cast<int>(serverList[i].clientList.size()); j++)
			{
				process_client_connection(serverList[i].clientList[j], serverList[i], j);
			}
			//lets see if we can get a NEW client connection
			if (FD_ISSET(serverList[i].fd, &read_set))//if socket_fd pointed by iterator is ready for accepting ...//here we can instead ask the socket to send SIGIO when activity occurs ... see socket(7) man
				accept_new_client_connection(serverList[i]);
		}
		// std::cout<<"server list size: "<<serverList.size()<<" ";
		// std::cout<<"out\n\n";
	}
}