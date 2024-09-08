/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: valeriafedorova <valeriafedorova@studen    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 13:40:11 by bperez-a          #+#    #+#             */
/*   Updated: 2024/09/08 12:14:50 by valeriafedo      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include "includes.hpp"
#include "ServerConfig.hpp"

class ConfigParser {
public:
    static std::vector<ServerConfig> parse(const std::string& configFile);

private:
    ConfigParser() {}
};

#endif
