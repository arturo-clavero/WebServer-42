/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artclave <artclave@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/04 04:42:21 by artclave          #+#    #+#             */
/*   Updated: 2024/10/04 07:20:03 by artclave         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cgi.hpp"
#include "ClientSocket.hpp"

Cgi::Cgi() : 
	cgi_error_code("500"),
	cgi_error_message("Internal Server Error"),
	substate(START)
	{}

Cgi::~Cgi(){}

void	Cgi::process(ClientSocket &client)
{
	if (client.get_state() != CGI)
		return ;
	if (client.response.getCgiPath().empty())
	{
		client.state++;
		return ;
	}
	if (substate == START)
	{
		pipe(pipe_fd);
		client.multiplex->add(pipe_fd[0]);
		client.multiplex->add(pipe_fd[1]);
		substate++;
		return ;
	}
	execute_cgi(client);
	wait_cgi();
	correct_cgi(client);
	incorrect_cgi(client);
}

void	Cgi::execute_cgi(ClientSocket &client)
{
	if (substate != EXECUTECGI || !client.multiplex->ready_to_write(pipe_fd[1]))
		return ;
    cgi_pid = fork();
    if (cgi_pid == -1) {
        std::cerr << "ERROR: Fork failed. Errno: " << errno << " - " << strerror(errno) << std::endl;
        client.response = ResponseBuilder::buildErrorResponse(client.match_config, client.request, "500", "Internal Server Error");
		client.state = FILES;
		return ;
    } 
	else if (cgi_pid == 0) {
		close(pipe_fd[0]);
		dup2(pipe_fd[1], STDOUT_FILENO);
		close(pipe_fd[1]);
		std::string cgi_path = client.response.getCgiPath();
		const CGIConfig& cgiConfig = client.match_config.getCgi();
 		std::string request_body = client.request.getBody(); // Store the body in a variable
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
		client.multiplex->remove(pipe_fd[1]);
		substate++;
		start_time = clock();
		client.read_operations++;
	}
}

void	Cgi::wait_cgi()
{
	if (substate != WAITCGI)
		return;
	int status;
	if (waitpid(cgi_pid, &status, WNOHANG) == 0)
	{
		if ((clock() - start_time)/CLOCKS_PER_SEC < MAX_TIME_CGI)
			return ;
		kill(cgi_pid, SIGINT);
		cgi_error_code = "504";
		cgi_error_message = "Gateway Timeout";
	}
	if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
		substate = CORRECT_CGI;
	else
		substate = INCORRECT_CGI;
	if (WIFEXITED(status) && WEXITSTATUS(status) == 2)
	{
		cgi_error_code = "403";
		cgi_error_message = "Forbidden";
	}
}

void	Cgi::incorrect_cgi(ClientSocket &client)
{
	if (substate != INCORRECT_CGI)
		return;
	client.read_operations = 0;
	client.multiplex->remove(pipe_fd[0]);
	client.response =  ResponseBuilder::buildErrorResponse(client.match_config, client.request, cgi_error_code, cgi_error_message);
	client.write_buffer = client.response.toString();
	if (!client.response.getFilePathForBody().empty())
		client.file_fd = open(client.response.getFilePathForBody().c_str(), O_RDONLY);
	client.state = FILES;
}

void	Cgi::correct_cgi(ClientSocket &client)
{
	if (substate != CORRECT_CGI || client.read_operations > 0 || !client.multiplex->ready_to_read(pipe_fd[0]))
		return ;
	char buff[READ_BUFFER_SIZE];
	memset(buff, 0, READ_BUFFER_SIZE);
	int bytes = read(pipe_fd[0], buff, READ_BUFFER_SIZE);
	if (bytes == -1)
		return ;
	client.read_operations++;
	for (int i = 0; i < bytes; i++)
		client.write_buffer += buff[i];
	if (bytes < READ_BUFFER_SIZE)
	{
		client.multiplex->remove(pipe_fd[0]);
		client.state = WRITE;
	}
}
