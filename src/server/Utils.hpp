/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artclave <artclave@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/01 14:30:45 by artclave          #+#    #+#             */
/*   Updated: 2024/10/07 06:32:16 by artclave         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

#include "../includes.hpp"
#include "../config/ServerConfig.hpp"

class	Utils
{
	public:
		static int			extract_port(const std::string &str);
		static uint32_t		extract_host(const std::string &str);
		static ServerConfig	find_match_config(Configs &possible_configs, const std::string host);
		static bool			is_found(std::size_t &result, std::string needle, std::string &haystack);
		static bool			is_found(std::string needle, std::string &haystack);
		static void			exit_on_error(std::string mssg);
};

#endif