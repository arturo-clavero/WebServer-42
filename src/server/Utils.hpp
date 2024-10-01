/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artclave <artclave@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/01 14:30:45 by artclave          #+#    #+#             */
/*   Updated: 2024/10/01 15:19:13 by artclave         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

#include "includes.hpp"

class	Utils
{
	public:
		static int			extract_port(const std::string &str);
		static uint32_t		extract_host(const std::string &str);	
};

#endif