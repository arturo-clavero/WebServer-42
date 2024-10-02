/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artclave <artclave@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/30 21:44:25 by artclave          #+#    #+#             */
/*   Updated: 2024/10/02 19:18:21 by artclave         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClientSocket.hpp"
#include "ServerCore.hpp"
#include "ServerSocket.hpp"
#include "Utils.hpp"

ClientSocket::ClientSocket(Multiplex *server_multiplex, int fd_) : multiplex(server_multiplex), fd(fd_), state(0), write_offset(0) {}

ClientSocket::~ClientSocket(){}

int	ClientSocket::get_fd() const { return fd; }
int	ClientSocket::get_state() const { return state; }

void	ClientSocket::process_connection(ServerSocket &server)
{
	read_operations = 0;
	write_operations = 0;
	read_request();
	init_http_process(server.get_possible_configs());
	execute_cgi();
	wait_cgi();	
	manage_files();
	write_response();
}

void	ClientSocket::read_request() //STATES
{
	if (state != READING) 
		return ;
	if (!multiplex->ready_to_read(fd))
	{
	//	std::cout<<"client not ready for reading ... \n";
		return ;
	}
	char buff[READ_BUFFER_SIZE];
	memset(buff, 0, READ_BUFFER_SIZE);
	std::size_t	pos_zero, pos_content_length, pos_header_end;
	int bytes = recv(fd, buff, READ_BUFFER_SIZE, 0);
	//std::cout<<"bytes: "<<bytes<<"\n";
	if (bytes == 0)
	{
		state = DISCONNECT;
		return ;
	}
	if (bytes == -1)
	{
		//strerror(errno);
		return ;
	}
	read_operations++;
	for (int i = 0; i < bytes; i++)
		read_buffer += buff[i];
	pos_header_end = read_buffer.find("\r\n\r\n");
    if (pos_header_end == std::string::npos)// Header is incomplete, wait for more data
	{
		//setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (void *)&multiplex->timeout, sizeof(multiplex->timeout));
    	return;
	}
	pos_content_length = read_buffer.find("Content-Length:");
	if (pos_content_length != std::string::npos)
	{
		long expected_body_size = std::atol(read_buffer.substr(pos_content_length + 16, pos_header_end).c_str());
		if (static_cast<int>(read_buffer.size() - pos_header_end - 4) < expected_body_size)// Body is incomplete, wait for more data
        {
			//setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (void *)&multiplex->timeout, sizeof(multiplex->timeout));
			return;
		}
	}
	else if (read_buffer.find("Transfer-Encoding: chunked") != std::string::npos)
	{
		pos_zero = read_buffer.find("0\r\n\r\n");
		if (pos_zero == std::string::npos) // Chunked body is incomplete, wait for more data
        {
			//setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (void *)&multiplex->timeout, sizeof(multiplex->timeout));
			return ;
		}
	}
	state++;
}

void	ClientSocket::init_http_process(std::vector<ServerConfig> &possible_configs)//STATES
{
	if (state != HTTP)
		return ;
	request = RequestParser::parse(read_buffer);
	read_buffer.clear();
	find_match_config(possible_configs, request.getHost());
	response = ResponseBuilder::build(request, match_config);
	//std::cout << "DEBUG: Response built" << std::endl;
	if (!response.getFilePathForBody().empty())
	{
		//std::cout<<"file name"<<response.getFilePathForBody().c_str()<<"\n";
		file_fd = open(response.getFilePathForBody().c_str(), O_RDONLY);
	}	
	state++;
}

void	ClientSocket::find_match_config(std::vector<ServerConfig> &possible_configs, const std::string host)//UTILS
{
	std::vector<std::string> possible_names;
	match_config = possible_configs[0];
	for (int i = 0; i < static_cast<int>(possible_configs.size()); i++)
	{
		possible_names = possible_configs[i].getServerNames();
		for (int j = 0; j < static_cast<int>(possible_names.size()); j++)
		{
			if (possible_names[j] == host)
			{
				match_config = possible_configs[i];
				return ;
			}
		}
	}
}


void	ClientSocket::execute_cgi()//STATES
{
	if (state != EXECUTECGI)
		return ;
	if (response.getCgiPath().empty())
	{
		state = FILES;
		return ;
	}
	if (write_operations > 0 || !multiplex->ready_to_write(fd))
		return;
    cgi_pid = fork();
    if (cgi_pid == -1) {
        std::cerr << "ERROR: Fork failed. Errno: " << errno << " - " << strerror(errno) << std::endl;
        response = ResponseBuilder::buildErrorResponse(match_config, request, "500", "Internal Server Error");
		state = FILES;
		return ;
    } 
	else if (cgi_pid == 0) {
      //  std::cout << "DEBUG: Child process. Executing CGI script" << std::endl;
		dup2(fd, STDOUT_FILENO);
		std::string cgi_path = response.getCgiPath();
		const CGIConfig& cgiConfig = match_config.getCgi();
 		std::string request_body = request.getBody(); // Store the body in a variable
		char* args[] = {
			const_cast<char*>(cgiConfig.path.c_str()),  // Python interpreter path
			const_cast<char*>(cgi_path.c_str()),        // Script path
			const_cast<char*>(request_body.c_str()),    // Argument to the script
			NULL
		};
		execv(cgiConfig.path.c_str(), args);
        std::cerr << "ERROR: execv failed. Errno: " << errno << " - " << strerror(errno) << std::endl;
        exit(1);
    }
	else
	{
		write_operations++;
		read_operations++;
		state++;
	}
}

void	ClientSocket::wait_cgi()//STATES
{
	if (state != WAITCGI)
		return;
	int status;
	if (waitpid(cgi_pid, &status, WNOHANG) == 0)
		return ;
	if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
		//std::cout << "DEBUG: CGI program executed successfully" << std::endl;
		//response = RequestResponse();
		//request = HttpRequest();
		state = DISCONNECT;
	} else {
		//std::cerr << "ERROR: CGI program failed. Exit status: " << WEXITSTATUS(status) << std::endl;
		response =  ResponseBuilder::buildErrorResponse(match_config, request, "500", "Internal Server Error");
		state = WRITE;
		write_buffer = response.toString();
		//response = RequestResponse();
		//request = HttpRequest();
	}
}

void	ClientSocket::manage_files()//STATES
{
	if (state != FILES)
		return ;
	//std::cout<<"MANAGING FILES\n";
	//std::cout<<"file fd: "<<file_fd<<"\n";
	if (!response.getFilePathForBody().empty()) 
	{
		//std::cout<<"check1\n";
		if (file_fd < 0)
			return ;
		if (read_operations > 0)
			return ;
		//std::cout<<"check2\n";
		int body_done = response.buildBodyFromFile(match_config, file_fd);
		read_operations ++;
		//std::cout<<"body done: "<<body_done<<"\n";
		if (body_done == false)
			return ;
		close(file_fd);
		//std::cout<<"check3\n";
	}
	if (request.hasPostFileContents() && request.hasPostFileFds())
	{
		if (write_operations > 0)
			return ;
		int max = static_cast<int>(write_buffer.size()) - write_offset;
		if (max > WRITE_BUFFER_SIZE)
			max = WRITE_BUFFER_SIZE;
		int bytes = write(request.getLastFileFd(), &(request.getLastFileContent())[write_offset], max);
		if (bytes < 0)
			return ; //some error saving teh file what to do here?
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
	//static int z;
	//std::cout<<++z<<" RESPONSE:\n"<<write_buffer<<"\n";
	state++;
}

void	ClientSocket::write_response()//STATES
{
	if (state != WRITE || write_operations > 0 || !multiplex->ready_to_write(fd))
		return ;
	//setsockopt(client.fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
	int max = static_cast<int>(write_buffer.size()) - write_offset;
	if (max > WRITE_BUFFER_SIZE)
		max = WRITE_BUFFER_SIZE;
	int bytes = send(fd, &write_buffer[write_offset], max, 0);
	if (bytes < 0)
	{
		 state = DISCONNECT;
		 return ;
	}
	write_operations++;
	write_offset += bytes;
	if (write_offset >= static_cast<int>(write_buffer.size()))
	{
		state = DISCONNECT;
		return ;
	}
}
