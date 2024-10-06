/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artclave <artclave@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/04 04:41:36 by artclave          #+#    #+#             */
/*   Updated: 2024/10/07 05:08:54 by artclave         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP 
# define CGI_HPP 

#include "../includes.hpp"
class ClientSocket;

enum	substate
{
	START,
	EXECUTECGI,
	WAITCGI,
	CORRECT_CGI,
	INCORRECT_CGI,
};

class Cgi
{
	private:
		pid_t			cgi_pid;
		clock_t			start_time;
		int				pipe_fd[2];
		std::string		cgi_error_code;
		std::string		cgi_error_message;
		int				substate;
		
		void	execute_cgi(ClientSocket &client);
		void	wait_cgi();
		void	correct_cgi(ClientSocket &client);
		void	incorrect_cgi(ClientSocket &client);

	public:
		Cgi();
		~Cgi();
		void	process(ClientSocket  &this_client);
};

#endif