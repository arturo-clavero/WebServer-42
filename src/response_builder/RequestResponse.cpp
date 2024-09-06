/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestResponse.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bperez-a <bperez-a@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/06 10:17:15 by bperez-a          #+#    #+#             */
/*   Updated: 2024/09/06 10:27:41 by bperez-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestResponse.hpp"

RequestResponse::RequestResponse() {
}

RequestResponse::~RequestResponse() {
}

std::string RequestResponse::toString() const {
        // Read the content of www/index.html
        std::ifstream file("www/index.html");
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        std::stringstream ss;
        ss << content.length();
        std::string contentLengthStr = ss.str();
        std::string response;
        response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + contentLengthStr + "\r\n\r\n" + content;
	return response;
}

