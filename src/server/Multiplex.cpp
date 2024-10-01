/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Multiplex.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artclave <artclave@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/01 07:07:40 by artclave          #+#    #+#             */
/*   Updated: 2024/10/01 14:35:57 by artclave         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Multiplex.hpp"

Multiplex::Multiplex() { timeout.tv_sec = 1; timeout.tv_usec = 0; }

Multiplex::~Multiplex(){}

bool	Multiplex::ready_to_read(int fd){ return (FD_ISSET(fd, &read_set));}

bool	Multiplex::ready_to_write(int fd){ return (FD_ISSET(fd, &write_set));}

void	Multiplex::add(int fd) { active_fds.push_back(fd); }

void	Multiplex::remove(int fd) 
{ 
	active_fds.remove(fd);
	FD_CLR(fd, &read_set);
	FD_CLR(fd, &write_set);
}
void	Multiplex::close_all_active_fds()
{
	for (std::list<int>::iterator it = active_fds.begin(); it != active_fds.end(); it++)
		close(*it);
}

void	Multiplex::reset_select()
{
	FD_ZERO(&read_set);
	FD_ZERO(&write_set);
	for (std::list<int>::iterator it = active_fds.begin(); it != active_fds.end(); it++)
	{
		FD_SET(*it, &read_set);
		FD_SET(*it, &write_set);
	}
	select(*max_element(active_fds.begin(), active_fds.end()) + 1, &read_set, &write_set, 0, &timeout);
}