/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bperez-a <bperez-a@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/05 16:33:26 by artclave          #+#    #+#             */
/*   Updated: 2024/09/09 13:53:08 by bperez-a         ###   ########.fr       */
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
    std::set<std::pair<std::string, int > > unique; // automatically prevents duplicate host-port combination --- but why ? We nned a socket per server even if they have duplicate host-port... if they have a different server-name then we need different sockets... Arturo asking
    std::vector<ServerConfig>& config;
    Server(std::vector<ServerConfig>& config);
    ~Server();
    void	run();
    void    createUniquePair();
    void    createEventManager();
    // void	check_non_blocking(int sockfd);
    // std::vector<ServerConfig>& getConfig() ;
	std::vector<int> create_listen_sockets(struct socaddr &addr);
};

#endif