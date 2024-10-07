/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artclave <artclave@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/30 21:44:25 by artclave          #+#    #+#             */
/*   Updated: 2024/10/07 07:25:02 by artclave         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClientSocket.hpp"
#include "ServerCore.hpp"
#include "ServerSocket.hpp"
#include "Utils.hpp"

ClientSocket::ClientSocket(Multiplex *server_multiplex, int fd_) : 
	multiplex(server_multiplex),
	fd(fd_), 
	state(0), 
	write_offset(0)
	{}

ClientSocket::~ClientSocket(){}

int	ClientSocket::get_fd() const { return fd; }
int	ClientSocket::get_state() const { return state; }

void	ClientSocket::process_connection(ServerSocket &server)
{
	read_operations = 0;
	write_operations = 0;
	read_request();
	init_http_process(server.get_possible_configs());
	cgi.process(*this);
	manage_files();
	write_response();
}

void	ClientSocket::read_request()
{
	if (state != READING || !multiplex->ready_to_read(fd))
		return ;
	char buff[MAX_BUFFER_SIZE];
	memset(buff, 0, MAX_BUFFER_SIZE);
	int bytes = recv(fd, buff, MAX_BUFFER_SIZE, 0);
	if (Utils::read_write_error(bytes, &state))
		return ;
	read_operations++;
	for (int i = 0; i < bytes; i++)
		read_buffer += buff[i];
	if (!Utils::complete_http_message(read_buffer))
		return;
	state++;
}

void	ClientSocket::init_http_process(Configs &possible_configs)
{
	if (state != HTTP)
		return ;
	request = RequestParser::parse(read_buffer);
	read_buffer.clear();
	match_config = Utils::find_match_config(possible_configs, request.getHost());
	response = ResponseBuilder::build(request, match_config);
	if (!response.getFilePathForBody().empty())
	{
		file_fd = open(response.getFilePathForBody().c_str(), O_RDONLY);
	}	
	state++;
}

void	ClientSocket::manage_files()
{
	if (state != FILES)
		return ;
	if (!response.getFilePathForBody().empty()) 
	{
		if (file_fd < 0)
			return ;
		if (read_operations > 0)
			return ;
		int body_done = response.buildBodyFromFile(match_config, file_fd, &state);
		read_operations ++;
		if (body_done == false)
			return ;
		close(file_fd);
	}
	if (request.hasPostFileContents() && request.hasPostFileFds())
	{
		if (write_operations > 0)
			return ;
		int max = static_cast<int>(request.getLastFileContent().size()) - write_offset;
		if (max > MAX_BUFFER_SIZE)
			max = MAX_BUFFER_SIZE;
		int bytes = write(request.getLastFileFd(), &(request.getLastFileContent())[write_offset], max);
		if (Utils::read_write_error(bytes, &state))
			return;
		write_operations++;
		write_offset += bytes;
		if (write_offset < static_cast<int>(request.getLastFileContent().size()))
			return ;
		write_offset = 0;
		close(request.getLastFileFd());
		request.popBackPostFileFds();
		request.popBackPostFileContents();
		if (request.hasPostFileContents() && request.hasPostFileFds())
			return ;
	}
	write_buffer = response.toString();
	state++;
}

void	ClientSocket::write_response()
{
	if (state != WRITE || write_operations > 0 || !multiplex->ready_to_write(fd))
		return ;

	int max = static_cast<int>(write_buffer.size()) - write_offset;
	if (max > MAX_BUFFER_SIZE)
		max = MAX_BUFFER_SIZE;
	int bytes = send(fd, &write_buffer[write_offset], max, 0);
	if (Utils::read_write_error(bytes, &state))
		return ;
	write_operations++;
	write_offset += bytes;
	if (write_offset >= static_cast<int>(write_buffer.size()))
		state = DISCONNECT;
}
