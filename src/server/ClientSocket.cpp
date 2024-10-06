/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artclave <artclave@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/30 21:44:25 by artclave          #+#    #+#             */
/*   Updated: 2024/10/07 05:04:34 by artclave         ###   ########.fr       */
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
	char buff[READ_BUFFER_SIZE];
	memset(buff, 0, READ_BUFFER_SIZE);
	int bytes = recv(fd, buff, READ_BUFFER_SIZE, 0);
	if (bytes == 0)
	{
		state = DISCONNECT;
		return ;
	}
	if (bytes == -1)
	{
		return ;
	}
	read_operations++;
	for (int i = 0; i < bytes; i++)
		read_buffer += buff[i];
	std::size_t	header, content_length;
	if (!Utils::is_found(header, "\r\n\r\n", read_buffer))
		return;
	if (Utils::is_found(content_length, "Content-Length:", read_buffer))
	{
		long expected_body_size = std::atol(read_buffer.substr(content_length + 16, header).c_str());
		long current_body_size = static_cast<int>(read_buffer.size() - header - 4);
		if (current_body_size < expected_body_size)
			return ;
	}
	else if (Utils::is_found("Transfer-Encoding: chunked", read_buffer) \
			&& !Utils::is_found("0\r\n\r\n", read_buffer))
		return ;
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
		//std::cout<<"POST!\n";
		if (write_operations > 0)
			return ;
		int max = static_cast<int>(request.getLastFileContent().size()) - write_offset;
		if (max > WRITE_BUFFER_SIZE)
			max = WRITE_BUFFER_SIZE;
		//std::cout<<"max: "<<max<<"\n";
		int bytes = write(request.getLastFileFd(), &(request.getLastFileContent())[write_offset], max);
		if (bytes == 0)
		{
			state = DISCONNECT;
			return ;
		}
		if (bytes == -1)
			return ;
		write_operations++;
				//std::cout<<"write offset: "<<write_offset<<"\n";
		write_offset += bytes;
		if (write_offset < static_cast<int>(request.getLastFileContent().size()))
			return ;
		std::cout<<"write offset + bytes: "<<write_offset<<"\n";
		write_offset = 0;
		close(request.getLastFileFd());
		request.popBackPostFileFds();
		request.popBackPostFileContents();
		if (request.hasPostFileContents() && request.hasPostFileFds())
			return ;
	}
	write_buffer = response.toString();
	std::cout<<"WRITE BUFFER IS : "<<write_buffer<<"\n";
	state++;
}

void	ClientSocket::write_response()
{
	if (state != WRITE || write_operations > 0 || !multiplex->ready_to_write(fd))
		return ;

	int max = static_cast<int>(write_buffer.size()) - write_offset;
	if (max > WRITE_BUFFER_SIZE)
		max = WRITE_BUFFER_SIZE;
	int bytes = send(fd, &write_buffer[write_offset], max, 0);
	if (bytes == 0)
	{
		state = DISCONNECT;
		return ;
	}
	if (bytes == -1)
		return ;
	write_operations++;
	write_offset += bytes;
	if (write_offset >= static_cast<int>(write_buffer.size()))
		state = DISCONNECT;
}
