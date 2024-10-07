/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artclave <artclave@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/01 14:31:51 by artclave          #+#    #+#             */
/*   Updated: 2024/10/07 07:40:47 by artclave         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Utils.hpp"

int	Utils::extract_port(const std::string &str){
	std::string::const_iterator semi_colon = std::find(str.begin(), str.end(), ':');
	if (semi_colon == str.end())
		return 8080;
	int pos = std::distance(str.begin(), semi_colon);
	std::string	port_string = str.substr(pos + 1);
	return (std::atoi(port_string.c_str()));
}

uint32_t	Utils::extract_host(const std::string &str){
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

ServerConfig	Utils::find_match_config(Configs &possible_configs, const std::string host)
{
	std::vector<std::string> possible_names;
	for (int i = 0; i < static_cast<int>(possible_configs.size()); i++)
	{
		possible_names = possible_configs[i].getServerNames();
		for (int j = 0; j < static_cast<int>(possible_names.size()); j++)
		{
			if (possible_names[j] == host)
				return possible_configs[i];
		}
	}
	return possible_configs[0];
}

bool	Utils::is_found(std::size_t &result, std::string needle, std::string &haystack)
{
	result = haystack.find(needle);
	return (result != std::string::npos);
}

bool	Utils::is_found(std::string needle, std::string &haystack)
{
	return (haystack.find(needle) != std::string::npos);
}

bool	Utils::error(std::string mssg, int should_exit)
{
	std::cerr<<mssg<<"\n";
	if (should_exit == EXIT)
		exit(1);
	return false;
}

bool	Utils::read_write_error(int bytes, int *state)
{
	if (bytes == 0)
	{
		*state = DISCONNECT;
		return true;
	}
	if (bytes == -1)
	{
		return true;
	}
	return false;
}

bool	Utils::complete_http_message(std::string &buffer)
{
	std::size_t	header, content_length;
	if (!Utils::is_found(header, "\r\n\r\n", buffer))
		return false;
	if (Utils::is_found(content_length, "Content-Length:", buffer))
	{
		long expected_body_size = std::atol(buffer.substr(content_length + 16, header).c_str());
		long current_body_size = static_cast<int>(buffer.size() - header - 4);
		if (current_body_size < expected_body_size)
			return false;
	}
	else if (Utils::is_found("Transfer-Encoding: chunked", buffer) \
			&& !Utils::is_found("0\r\n\r\n", buffer))
		return false;
	return true;
}