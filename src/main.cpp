/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bperez-a <bperez-a@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 20:09:33 by bperez-a          #+#    #+#             */
/*   Updated: 2024/09/06 09:32:55 by bperez-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "includes.hpp"

#include "server/Server.hpp"
#include "config/ConfigParser.hpp"
#include "config/ServerConfig.hpp"
#include "request_parser/HttpRequest.hpp"
#include "response_builder/ResponseBuilder.hpp"


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <config_file_path>" << std::endl;
        return 1;
    }

    ConfigParser parser(argv[1]);
    ServerConfig config = parser.parse();
	config.print();
	Server server(config);
	server.run();
    return 0;
}
