/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseBuilder.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bperez-a <bperez-a@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/06 10:15:30 by bperez-a          #+#    #+#             */
/*   Updated: 2024/09/06 15:30:28 by bperez-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ResponseBuilder.hpp"

RequestResponse ResponseBuilder::build(const HttpRequest& request, const ServerConfig& config) {
	
	RequestResponse response;
	//for location in serverconfig.locations
	//if request.getpath matches location.path, which means either is location.path or starts with location.path and is followed by /
	// if matches location.path : serves the index of that location, by constructing location.root+location.path+location.index. If the location has no index and autoindex is on, then list the files in the location.path
	//if starts with location.path and is followed by / AND something else, check if that file exists at that location by constructing location.root+location.path+request.getpath. If it does, serve the file. If it doesn't, return 404

	for (size_t i = 0; i < config.getLocations().size(); i++)
	{
		if (request.getPath() == config.getLocations()[i].path)
		{
			response.setStatusCode("200");
			response.setStatusMessage("OK");
			response.setContentType("text/html");
			std::string filePath = config.getLocations()[i].root + "/" + config.getLocations()[i].index;
			std::ifstream file(filePath.c_str());
			std::stringstream buffer;
			buffer << file.rdbuf();
			response.setBody(buffer.str());
			std::ostringstream oss;
			oss << response.getBody().length();
			response.setContentLength(oss.str());
			return response;
			//todo : if no index and autoindex is on, list the files in the location.path. If no index and autoindex is off, return 403
		}
		else if (request.getPath().find(config.getLocations()[i].path + "/") == 0)
		{
			std::string filePath = config.getLocations()[i].root + request.getPath();
			std::ifstream file(filePath.c_str());
			if (file.good())
			{
				response.setStatusCode("200");
				response.setStatusMessage("OK");
				std::string extension = request.getPath().substr(request.getPath().find_last_of(".") + 1);
				response.setContentType(request.getType() + "/" + extension); 
				std::stringstream buffer;
				buffer << file.rdbuf();
				response.setBody(buffer.str());
				std::ostringstream oss;
				oss << response.getBody().length();
				response.setContentLength(oss.str());
				return response;
			}
			else
			{
				response.setStatusCode("404");
				response.setStatusMessage("Not Found");
				response.setContentType("text/html");
				std::ifstream file("www/error/404.html");
				std::stringstream buffer;
				buffer << file.rdbuf();
				response.setBody(buffer.str());
				std::ostringstream oss;
				oss << response.getBody().length();
				response.setContentLength(oss.str());
				return response;
			}
		}
	}
	return response;
}
