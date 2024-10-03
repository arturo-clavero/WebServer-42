/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseBuilder.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artclave <artclave@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/06 10:15:30 by bperez-a          #+#    #+#             */
/*   Updated: 2024/10/03 16:14:26 by artclave         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ResponseBuilder.hpp"
#include "ResponseUtils.hpp"
#include <iostream>

RequestResponse ResponseBuilder::build(HttpRequest& request, ServerConfig& config) {
	RequestResponse response;
	if (ResponseUtils::isRequestTooLarge(request, config.getClientMaxBodySize()) == true)
	{
		response = buildErrorResponse(config, request, "413", "Request Entity Too Large");
		return response;
	}
	if (ResponseUtils::isRequestValid(request) == false)
	{
		response = buildErrorResponse(config, request, "400", "Bad Request");
		return response;
	}
	//if CGI request, build CGI response
	if (ResponseUtils::isCGIRequest(config, request) == true)
	{
		response = buildCGIResponse(config, request);
		return response;
	}
	//find location
	LocationConfig location = ResponseUtils::findLocation(request.getPath(), config);

	//if location is not found, build error response
	if (location.root.empty())
	{
		response = buildErrorResponse(config, request, "404", "Not Found");
		return response;
	}
	
	//build response based on the method
	if (request.getMethod() == "GET") {
		response = buildGetResponse(config, request, location);
	} else if (request.getMethod() == "POST") {
		response = buildPostResponse(config, request, location);
	} else if (request.getMethod() == "DELETE") {
		response = buildDeleteResponse(config, request, location);
	} else {
		//if method is not implemented, build error response
		response = buildErrorResponse(config, request, "501", "Not Implemented");
	}
	return response;
}


RequestResponse ResponseBuilder::buildGetResponse(ServerConfig& config, HttpRequest& request, LocationConfig& location) {
	RequestResponse response;

	//check if method is allowed
	if (ResponseUtils::isMethodAllowed(request, location) == false)
	{
		//if method is not allowed, build error response
		response = buildErrorResponse(config, request, "405", "Method Not Allowed");
		return response;
	}
	
	//build response based on the target type
	FileType targetType = ResponseUtils::getTargetType(request);
	if (targetType == IS_FILE)
		response = buildGetFileResponse(config, request, location);
	else if (targetType == IS_DIRECTORY)
		response = buildGetDirectoryResponse(config, request, location);
	else
		response = buildErrorResponse(config, request, "404", "Not Found");
	return response;
}

RequestResponse ResponseBuilder::buildGetFileResponse(ServerConfig& config, HttpRequest& request, LocationConfig& location) {
	(void)location;
	RequestResponse response;
	//check if file exists
	std::string path = location.root + request.getPath();	
	if (access(path.c_str(), F_OK) == -1)
	{
		response = buildErrorResponse(config, request, "404", "Not Found");
		return response;
	}
	if (access(path.c_str(), R_OK) == -1)
	{
		response = buildErrorResponse(config, request, "403", "Forbidden");
		return response;
	}
	//build success response
	response = buildSuccessResponse(config, request, location, path);
	return response;
}

RequestResponse ResponseBuilder::buildGetDirectoryResponse(ServerConfig& config, HttpRequest& request, LocationConfig& location) {
	RequestResponse response;
	//check if directory exists
	std::string path = location.root + request.getPath();
	if (access(path.c_str(), F_OK) == -1)
	{
		response = buildErrorResponse(config, request, "404", "Not Found");
		return response;
	}
	//check if an index is specified at the location
	if (location.index.empty() == true)
	{
		// if index is not specified and autoindex is true, build autoindex response
		if (location.autoindex == true)
		{
			response = buildAutoindexResponse(config, request, location, path);
			return response;
		}
		// if index is not specified and autoindex is false, build error response
		else 
		{
			response = buildErrorResponse(config, request, "403", "Forbidden");
			return response;
		}
	}
	
	//make index path
	std::string index_path = location.root + "/" + location.index;
	
	//check if index exists
	if (access(index_path.c_str(), F_OK) == -1)
	{
		// if index path does not exist and autoindex is true, build autoindex response
		if (location.autoindex == true)
		{
			std::string path = location.root + request.getPath();
			response = buildAutoindexResponse(config, request, location, path);
			return response;
		}
		// if index path does not exist and autoindex is false, build error response
		else
		{
			response = buildErrorResponse(config, request, "403", "Forbidden");
			return response;
		}
	}
	
	// build get file response with the location index
	request.setPath("/" + location.index);
	response = buildGetFileResponse(config, request, location);
	return response;
}


RequestResponse ResponseBuilder::buildErrorResponse(ServerConfig& config, HttpRequest& request, const std::string& code, const std::string& message) {
	(void)request;
	RequestResponse response;
	response.setStatusCode(code);
	response.setStatusMessage(message);
	//check if error page is specified at the server
	if (config.getErrorPages().find(code) != config.getErrorPages().end())
	{
		std::cout<<"getting page .. \n";
		response.setContentType("text/html");
		std::string path = config.getRoot() + config.getErrorPages().find(code)->second;
		std::cout<<"path: "<<path<<"\n";
		response.setFilePathForBody(path);
		response.setContentLengthFromPath(path);
	}
	else
	{
		std::cout<<"COULDNT FIND ERROR PAGE \n";
	}
	return response;
}

RequestResponse ResponseBuilder::buildSuccessResponse(ServerConfig& config, HttpRequest& request, LocationConfig& location, std::string& path) {
    (void)location;
    (void)request;
	(void)config;
    RequestResponse response;
    response.setStatusCode("200");
    response.setStatusMessage("OK");
	response.setFilePathForBody(path);
	response.setContentLengthFromPath(path);

    // Set content type based on file extension
    std::string contentType = ResponseUtils::getContentType(path);
    response.setContentType(contentType);

    // Determine if the file should be forced to download
    response.setContentDisposition("inline");
    return response;
}

RequestResponse ResponseBuilder::buildAutoindexResponse(ServerConfig& config, HttpRequest& request, LocationConfig& location, std::string& path) {
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
		while ((entry = readdir(dir)) != NULL) {
			std::string filename = entry->d_name;
			if (filename != "." && filename != "..") {
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
	return response;
}


RequestResponse ResponseBuilder::buildPostResponse(ServerConfig& config, HttpRequest& request, LocationConfig& location) {
    RequestResponse response;

    // Check if method is allowed
    if (ResponseUtils::isMethodAllowed(request, location) == false) {
        response = buildErrorResponse(config, request, "405", "Method Not Allowed");
        return response;
    }
    
    // Check if the target location exists and is writable
    std::string path = location.root + request.getPath();
    if (access(path.c_str(), W_OK) == -1) {
        response = buildErrorResponse(config, request, "403", "Forbidden");
        return response;
    }

    // Process the POST data (e.g., save uploaded file or process form data)
    if (!processPostData(request, path)) {
        response = buildErrorResponse(config, request, "500", "Internal Server Error");
        return response;
    }
    // Build success response
    response = buildPostSuccessResponse(config, request, location);
    return response;
}

RequestResponse ResponseBuilder::buildPostSuccessResponse(ServerConfig& config, HttpRequest& request, LocationConfig& location) {
	(void)config;
	(void)location;
	(void)request;
	RequestResponse response;
	response.setStatusCode("200");
	response.setStatusMessage("OK");
	response.setBody("POST response HELLO");
	response.setContentLength(response.getBody().length());
	return response;
}

//process post data
bool  ResponseBuilder::processPostData(HttpRequest& request, std::string& path) {
	//find boundary from content type
	std::string boundary = request.getHeader("Content-Type").substr(request.getHeader("Content-Type").find("boundary=") + 9);
	std::vector<PostRequestBodyPart> bodyParts = PostRequestBodySnatcher::parse(request.getBody(), boundary);
	for (std::vector<PostRequestBodyPart>::iterator it = bodyParts.begin(); it != bodyParts.end(); ++it)
	{
		//print filename
		if (it->getFilename().empty() == false)
			if (ResponseUtils::openFiles(path, it->getFilename(), it->getContent(), request) == false)
				return false;
	}
	return true;
}

RequestResponse ResponseBuilder::buildDeleteResponse(ServerConfig& config, HttpRequest& request, LocationConfig& location) {
	//check if method is allowed
	RequestResponse response;
	if (ResponseUtils::isMethodAllowed(request, location) == false)
	{
		response = buildErrorResponse(config, request, "405", "Method Not Allowed");
		return response;
	}
	//check if file exists
	std::string path = location.root + request.getPath();
	if (access(path.c_str(), F_OK) == -1)
	{
		response = buildErrorResponse(config, request, "404", "Not Found");
		return response;
	}
	//check if file is writable and not a directory
	FileType targetType = ResponseUtils::getTargetType(request);
	if (access(path.c_str(), W_OK) == -1 || targetType == IS_DIRECTORY)
	{
		response = buildErrorResponse(config, request, "403", "Forbidden");
		return response;
	}
	//delete file
	if (remove(path.c_str()) == -1)
	{
		response = buildErrorResponse(config, request, "500", "Internal Server Error");
		return response;
	}
	//build success response
	response.setStatusCode("200");
	response.setStatusMessage("OK");
	response.setBody("File deleted successfully");
	response.setContentLength(response.getBody().length());
	return response;
}

RequestResponse ResponseBuilder::buildCGIResponse(ServerConfig& config, HttpRequest& request) {
    const CGIConfig& cgiConfig = config.getCgi();
    std::string requestPath = request.getPath();
    std::string fullPath = cgiConfig.root + requestPath;
    // Check if the file exists
    if (access(fullPath.c_str(), F_OK) == -1) {
        return buildErrorResponse(config, request, "404", "Not Found");
    }
	RequestResponse response;
	response.setCgiPath(fullPath);
	return response;
}
