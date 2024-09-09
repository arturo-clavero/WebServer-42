/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bperez-a <bperez-a@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 20:09:33 by bperez-a          #+#    #+#             */
/*   Updated: 2024/09/09 14:09:39 by bperez-a         ###   ########.fr       */
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

	std::vector<ServerConfig> config = ConfigParser::parse(argv[1]);
	
	//print each one
	for (size_t i = 0; i < config.size(); i++) {
		config[i].print();
	}
	try {
		Server server(config);
		server.run();	
	} catch (std::exception &e) {
		std::cout << e.what() << std::endl;
		return (1);
	}
    return 0;
}
