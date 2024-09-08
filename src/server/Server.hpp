/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: valeriafedorova <valeriafedorova@studen    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/05 16:33:26 by artclave          #+#    #+#             */
/*   Updated: 2024/09/08 12:34:56 by valeriafedo      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "includes.hpp"
#include "config/ConfigParser.hpp"
#include "config/ServerConfig.hpp"
#include "request_parser/HttpRequest.hpp"
#include "request_parser/RequestParser.hpp"
#include "response_builder/ResponseBuilder.hpp"

class Server {
private:
public:
    //unique pairs of host and ip address
    std::set<std::pair<std::string, int > > unique; // automatically prevents duplicate host-port combination
    std::vector<ServerConfig>& config;
    Server(std::vector<ServerConfig>& config);
    ~Server();
    void	run();
    void    createUniquePair();
    void    createEventManager();
    // void	check_non_blocking(int sockfd);
    // std::vector<ServerConfig>& getConfig() ;
	std::vector<int> Server::create_listen_sockets(struct socaddr &addr);
};

#endif