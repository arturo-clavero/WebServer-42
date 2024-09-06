/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseBuilder.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bperez-a <bperez-a@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/06 10:15:30 by bperez-a          #+#    #+#             */
/*   Updated: 2024/09/06 11:03:33 by bperez-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ResponseBuilder.hpp"

RequestResponse ResponseBuilder::build(const HttpRequest& request, const ServerConfig& config) {
	
	(void)config;
	RequestResponse response;
	if (request.getPath() == "/")
	{
		response.setStatusCode("200");
		response.setStatusMessage("OK");
		response.setContentType("text/html");
		std::ifstream file("www/index.html");
		std::stringstream buffer;
		buffer << file.rdbuf();
		response.setBody(buffer.str());
		std::ostringstream oss;
		oss << response.getBody().length();
		response.setContentLength(oss.str());
		
	}
	else
	{
		response.setStatusCode("200");
		response.setStatusMessage("OK");
		response.setContentType("text/html");
		std::ifstream file("www/error/404.html");
		std::stringstream buffer;
		buffer << file.rdbuf();
		response.setBody(buffer.str());
		std::ostringstream oss;
		oss << response.getBody().length();
		response.setContentLength(oss.str());
	}
	return response;

}