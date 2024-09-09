/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseBuilder.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bperez-a <bperez-a@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/06 10:15:30 by bperez-a          #+#    #+#             */
/*   Updated: 2024/09/09 14:48:56 by bperez-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ResponseBuilder.hpp"
#include "ResponseUtils.hpp"
#include <iostream>

RequestResponse ResponseBuilder::build(HttpRequest& request, ServerConfig& config) {
	std::cout << "DEBUG: Entering ResponseBuilder::build" << std::endl;
	
	RequestResponse response;
	//check if request is valid
	if (ResponseUtils::isRequestValid(request) == false)
	{
		std::cout << "DEBUG: Request is not valid" << std::endl;
		//if request is not valid, build error response
		response = buildErrorResponse(config, request, "400", "Bad Request");
		return response;
	}
	//find location
	LocationConfig location = ResponseUtils::findLocation(request.getPath(), config);

	//if location is not found, build error response
	if (location.root.empty())
	{
		std::cout << "DEBUG: Location not found" << std::endl;
		response = buildErrorResponse(config, request, "404", "Not Found");
		return response;
	}
	
	//build response based on the method
	std::cout << "DEBUG: Building response for method: " << request.getMethod() << std::endl;
	if (request.getMethod() == "GET") {
		response = buildGetResponse(config, request, location);
	} else if (request.getMethod() == "POST") {
		response = buildPostResponse(config, request, location);
	} else if (request.getMethod() == "DELETE") {
		response = buildDeleteResponse(config, request, location);
	} else {
		//if method is not implemented, build error response
		std::cout << "DEBUG: Method not implemented" << std::endl;
		response = buildErrorResponse(config, request, "501", "Not Implemented");
	}
	std::cout << "DEBUG: Exiting ResponseBuilder::build" << std::endl;
	return response;
}


RequestResponse ResponseBuilder::buildGetResponse(ServerConfig& config, HttpRequest& request, LocationConfig& location) {
	std::cout << "DEBUG: Entering ResponseBuilder::buildGetResponse" << std::endl;
	
	RequestResponse response;

	//check if method is allowed
	if (ResponseUtils::isMethodAllowed(request) == false)
	{
		std::cout << "DEBUG: Method not allowed" << std::endl;
		//if method is not allowed, build error response
		response = buildErrorResponse(config, request, "405", "Method Not Allowed");
		return response;
	}
	
	//build response based on the target type
	FileType targetType = ResponseUtils::getTargetType(request);
	std::cout << "DEBUG: Target type: " << targetType << std::endl;
	if (targetType == IS_FILE)
		response = buildGetFileResponse(config, request, location);
	else if (targetType == IS_DIRECTORY)
		response = buildGetDirectoryResponse(config, request, location);
	else
		response = buildErrorResponse(config, request, "404", "Not Found");
	
	std::cout << "DEBUG: Exiting ResponseBuilder::buildGetResponse" << std::endl;
	return response;
}

RequestResponse ResponseBuilder::buildGetFileResponse(ServerConfig& config, HttpRequest& request, LocationConfig& location) {
	std::cout << "DEBUG: Entering ResponseBuilder::buildGetFileResponse" << std::endl;
	(void)location;
	RequestResponse response;
	//check if file exists
	std::string path = location.root + request.getPath();	
	std::cout << "DEBUG: Path: " << path << std::endl;
	if (access(path.c_str(), F_OK) == -1)
	{
		std::cout << "DEBUG: File not found" << std::endl;
		response = buildErrorResponse(config, request, "404", "Not Found");
		return response;
	}
	//check if file readable
	if (access(path.c_str(), R_OK) == -1)
	{
		std::cout << "DEBUG: File not readable" << std::endl;
		response = buildErrorResponse(config, request, "403", "Forbidden");
		return response;
	}
	//build success response
	std::cout << "DEBUG: Building success response for file" << std::endl;
	response = buildSuccessResponse(config, request, location, path);
	std::cout << "DEBUG: Exiting ResponseBuilder::buildGetFileResponse" << std::endl;
	return response;
}

RequestResponse ResponseBuilder::buildGetDirectoryResponse(ServerConfig& config, HttpRequest& request, LocationConfig& location) {
	std::cout << "DEBUG: Entering ResponseBuilder::buildGetDirectoryResponse" << std::endl;
	RequestResponse response;
	//check if directory exists
	std::string path = location.root + request.getPath();
	std::cout << "DEBUG: Path: " << path << std::endl;
	if (access(path.c_str(), F_OK) == -1)
	{
		std::cout << "DEBUG: Directory not found" << std::endl;
		response = buildErrorResponse(config, request, "404", "Not Found");
		return response;
	}
	//check if an index is specified at the location
	if (location.index.empty() == true)
	{
		std::cout << "DEBUG: No index specified" << std::endl;
		// if index is not specified and autoindex is true, build autoindex response
		if (location.autoindex == true)
		{
			std::cout << "DEBUG: Building autoindex response" << std::endl;
			
			response = buildAutoindexResponse(config, request, location, path);
			return response;
		}
		// if index is not specified and autoindex is false, build error response
		else 
		{
			std::cout << "DEBUG: Autoindex not allowed" << std::endl;
			response = buildErrorResponse(config, request, "403", "Forbidden");
			return response;
		}
	}
	
	//make index path
	std::string index_path = location.root + "/" + location.index;
	std::cout << "DEBUG: Index path: " << index_path << std::endl;
	
	//check if index exists
	if (access(index_path.c_str(), F_OK) == -1)
	{
		std::cout << "DEBUG: Index file not found" << std::endl;
		// if index path does not exist and autoindex is true, build autoindex response
		if (location.autoindex == true)
		{
			std::cout << "DEBUG: Building autoindex response" << std::endl;
			std::string path = location.root + request.getPath();
			response = buildAutoindexResponse(config, request, location, path);
			return response;
		}
		// if index path does not exist and autoindex is false, build error response
		else
		{
			std::cout << "DEBUG: Autoindex not allowed" << std::endl;
			response = buildErrorResponse(config, request, "403", "Forbidden");
			return response;
		}
	}
	
	// build get file response with the location index
	std::cout << "DEBUG: Building get file response for index" << std::endl;
	request.setPath("/" + location.index);
	response = buildGetFileResponse(config, request, location);
	std::cout << "DEBUG: Exiting ResponseBuilder::buildGetDirectoryResponse" << std::endl;
	return response;
}


RequestResponse ResponseBuilder::buildErrorResponse(ServerConfig& config, HttpRequest& request, const std::string& code, const std::string& message) {
	std::cout << "DEBUG: Entering ResponseBuilder::buildErrorResponse" << std::endl;
	(void)request;
	RequestResponse response;
	response.setStatusCode(code);
	response.setStatusMessage(message);
	//check if error page is specified at the server
	if (config.getErrorPages().find(code) != config.getErrorPages().end())
	{
		std::cout << "DEBUG: Custom error page found for code: " << code << std::endl;
		response.setContentType("text/html");
		std::string path = config.getRoot() + config.getErrorPages().find(code)->second;
		response.setBody(ResponseUtils::buildBodyFromFile(config, path));
	}
	response.setContentLength((response.getBody().length()));
	std::cout << "DEBUG: Exiting ResponseBuilder::buildErrorResponse" << std::endl;
	return response;
}

RequestResponse ResponseBuilder::buildSuccessResponse(ServerConfig& config, HttpRequest& request, LocationConfig& location, std::string& path) {
    std::cout << "DEBUG: Entering ResponseBuilder::buildSuccessResponse" << std::endl;
    (void)location;
    (void)request;
    RequestResponse response;
    response.setStatusCode("200");
    response.setStatusMessage("OK");
    response.setBody(ResponseUtils::buildBodyFromFile(config, path));
    response.setContentLength(response.getBody().length());
    
    // Set content type based on file extension
    std::string contentType = ResponseUtils::getContentType(path);
    response.setContentType(contentType);

    // Determine if the file should be forced to download
    size_t dotPos = path.find_last_of('.');
    if (dotPos != std::string::npos) {
        std::string extension = path.substr(dotPos);
        if (ResponseUtils::shouldForceDownload(extension)) {
            std::string filename = path.substr(path.find_last_of("/\\") + 1);
            response.setContentDisposition("attachment; filename=\"" + filename + "\"");
        } else {
            response.setContentDisposition("inline");
        }
    }

    std::cout << "DEBUG: Content-Type set to: " << response.getContentType() << std::endl;
    std::cout << "DEBUG: Exiting ResponseBuilder::buildSuccessResponse" << std::endl;
    return response;
}

RequestResponse ResponseBuilder::buildAutoindexResponse(ServerConfig& config, HttpRequest& request, LocationConfig& location, std::string& path) {
	std::cout << "DEBUG: Entering ResponseBuilder::buildAutoindexResponse" << std::endl;
	(void)config;
	(void)location;

	RequestResponse response;
	response.setStatusCode("200");
	response.setStatusMessage("OK");
	response.setContentType("text/html");

	std::string body = "<html><head><title>Index of " + request.getPath() + "</title></head><body>";
	body += "<h1>Index of " + request.getPath() + "</h1><hr><ul style='list-style-type:none;'>";

	DIR *dir;
	struct dirent *entry;
	dir = opendir(path.c_str());
	if (dir != NULL) {
		std::cout << "DEBUG: Reading directory: " << path << std::endl;
		while ((entry = readdir(dir)) != NULL) {
			std::string filename = entry->d_name;
			if (filename != "." && filename != "..") {
				std::cout << "DEBUG: Adding file to autoindex: " << filename << std::endl;
				std::string href = request.getPath();
				if (!href.empty() && href[href.size() - 1] != '/') {
					href += '/';
				}
				href += filename;
				body += "<li><a href='" + href + "'>" + filename + "</a></li>";
			}
		}
		closedir(dir);
	}

	body += "</ul><hr></body></html>";
	response.setBody(body);
	response.setContentLength(response.getBody().length());
	std::cout << "DEBUG: Exiting ResponseBuilder::buildAutoindexResponse" << std::endl;
	return response;
}







RequestResponse ResponseBuilder::buildPostResponse(ServerConfig& config, HttpRequest& request, LocationConfig& location) {
	std::cout << "DEBUG: Entering ResponseBuilder::buildPostResponse" << std::endl;
	(void)config;
	(void)location;
	(void)request;
	RequestResponse response;
	response.setStatusCode("200");
	response.setStatusMessage("OK");
	response.setBody("POST response");
	response.setContentLength(response.getBody().length());
	std::cout << "DEBUG: Exiting ResponseBuilder::buildPostResponse" << std::endl;
	return response;
}

RequestResponse ResponseBuilder::buildDeleteResponse(ServerConfig& config, HttpRequest& request, LocationConfig& location) {
	std::cout << "DEBUG: Entering ResponseBuilder::buildDeleteResponse" << std::endl;
	(void)config;
	(void)location;
	(void)request;
	RequestResponse response;
	response.setStatusCode("200");	
	response.setStatusMessage("OK");
	response.setBody("DELETE response");
	response.setContentLength(response.getBody().length());
	std::cout << "DEBUG: Exiting ResponseBuilder::buildDeleteResponse" << std::endl;
	
	return response;
}