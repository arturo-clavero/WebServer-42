/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseBuilder.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bperez-a <bperez-a@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/06 10:15:30 by bperez-a          #+#    #+#             */
/*   Updated: 2024/09/25 12:44:48 by bperez-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ResponseBuilder.hpp"
#include "ResponseUtils.hpp"
#include <iostream>

RequestResponse ResponseBuilder::build(HttpRequest& request, ServerConfig& config) {
	std::cout << "DEBUG: Entering ResponseBuilder::build" << std::endl;
	
	RequestResponse response;
	if (ResponseUtils::isRequestTooLarge(request, config.getClientMaxBodySize()) == true)
	{
		std::cout << "DEBUG: Request is too large" << std::endl;
		response = buildErrorResponse(config, request, "413", "Request Entity Too Large");
		return response;
	}
	if (ResponseUtils::isRequestValid(request) == false)
	{
		std::cout << "DEBUG: Request is not valid" << std::endl;
		//if request is not valid, build error response
		response = buildErrorResponse(config, request, "400", "Bad Request");
		return response;
	}
	//if CGI request, build CGI response
	if (ResponseUtils::isCGIRequest(config, request) == true)
	{
		std::cout << "DEBUG: Request is a CGI request" << std::endl;
		response = buildCGIResponse(config, request);
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
	if (ResponseUtils::isMethodAllowed(request, location) == false)
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
		response.setFilePathForBody(path);
		response.setContentLengthFromPath(path);
	}
	std::cout << "DEBUG: Exiting ResponseBuilder::buildErrorResponse" << std::endl;
	return response;
}

RequestResponse ResponseBuilder::buildSuccessResponse(ServerConfig& config, HttpRequest& request, LocationConfig& location, std::string& path) {
    std::cout << "DEBUG: Entering ResponseBuilder::buildSuccessResponse" << std::endl;
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
    
    RequestResponse response;

    // Check if method is allowed
    if (ResponseUtils::isMethodAllowed(request, location) == false) {
        std::cout << "DEBUG: Method not allowed" << std::endl;
        response = buildErrorResponse(config, request, "405", "Method Not Allowed");
        return response;
    }
    
    // Check if the target location exists and is writable
    std::string path = location.root + request.getPath();
    if (access(path.c_str(), W_OK) == -1) {
        std::cout << "DEBUG: Target location not writable" << std::endl;
        response = buildErrorResponse(config, request, "403", "Forbidden");
        return response;
    }

    // Process the POST data (e.g., save uploaded file or process form data)
    if (!processPostData(request, path)) {
        std::cout << "DEBUG: Failed to process POST data" << std::endl;
        response = buildErrorResponse(config, request, "500", "Internal Server Error");
        return response;
    }
    // Build success response
    std::cout << "DEBUG: Building success response for POST" << std::endl;
    response = buildPostSuccessResponse(config, request, location);
	if (!request.getPostFileContents().empty() && !request.getPostFileFds().empty())
	{
		response.setPostFileContents(request.getPostFileContents());
		response.setPostFileFds(request.getPostFileFds());
	}
	std::cout << "DEBUG: Exiting ResponseBuilder::buildPostResponse" << std::endl;
    return response;
}

RequestResponse ResponseBuilder::buildPostSuccessResponse(ServerConfig& config, HttpRequest& request, LocationConfig& location) {
	std::cout << "DEBUG: Entering ResponseBuilder::buildPostSuccessResponse" << std::endl;
	(void)config;
	(void)location;
	(void)request;
	RequestResponse response;
	response.setStatusCode("200");
	response.setStatusMessage("OK");
	response.setBody("POST response HELLO");
	response.setContentLength(response.getBody().length());
	std::cout << "DEBUG: Exiting ResponseBuilder::buildPostSuccessResponse" << std::endl;
	return response;
}

//process post data
bool  ResponseBuilder::processPostData(HttpRequest& request, std::string& path) {
	std::cout << "DEBUG: Entering ResponseBuilder::processPostData" << std::endl;
	//find boundary from content type
	std::string boundary = request.getHeader("Content-Type").substr(request.getHeader("Content-Type").find("boundary=") + 9);
	std::cout << "DEBUG: Boundary: " << boundary << std::endl;
	std::vector<PostRequestBodyPart> bodyParts = PostRequestBodySnatcher::parse(request.getBody(), boundary);
	std::cout << "DEBUG: Body parts size: " << bodyParts.size() << std::endl;
	
	for (std::vector<PostRequestBodyPart>::iterator it = bodyParts.begin(); it != bodyParts.end(); ++it)
	{
		if (it->getFilename().empty() == false)
			if (ResponseUtils::openFiles(path, it->getFilename(), it->getContent(), request) == false)
				return false;
	}
	std::cout << "DEBUG: Exiting ResponseBuilder::processPostData" << std::endl;
	return true;
}

RequestResponse ResponseBuilder::buildDeleteResponse(ServerConfig& config, HttpRequest& request, LocationConfig& location) {
	std::cout << "DEBUG: Entering ResponseBuilder::buildDeleteResponse" << std::endl;
	//check if method is allowed
	RequestResponse response;
	if (ResponseUtils::isMethodAllowed(request, location) == false)
	{
		std::cout << "DEBUG: Method not allowed" << std::endl;
		response = buildErrorResponse(config, request, "405", "Method Not Allowed");
		return response;
	}
	//check if file exists
	std::string path = location.root + request.getPath();
	if (access(path.c_str(), F_OK) == -1)
	{
		std::cout << "DEBUG: File not found" << std::endl;
		response = buildErrorResponse(config, request, "404", "Not Found");
		return response;
	}
	//check if file is writable and not a directory
	FileType targetType = ResponseUtils::getTargetType(request);
	if (access(path.c_str(), W_OK) == -1 || targetType == IS_DIRECTORY)
	{
		std::cout << "DEBUG: File not writable" << std::endl;
		response = buildErrorResponse(config, request, "403", "Forbidden");
		return response;
	}
	//delete file
	if (remove(path.c_str()) == -1)
	{
		std::cout << "DEBUG: Failed to delete file" << std::endl;
		response = buildErrorResponse(config, request, "500", "Internal Server Error");
		return response;
	}
	//build success response
	response.setStatusCode("200");
	response.setStatusMessage("OK");
	response.setBody("File deleted successfully");
	response.setContentLength(response.getBody().length());
	std::cout << "DEBUG: Exiting ResponseBuilder::buildDeleteResponse" << std::endl;
	
	return response;
}

RequestResponse ResponseBuilder::buildCGIResponse(ServerConfig& config, HttpRequest& request) {
    std::cout << "DEBUG: Entering ResponseBuilder::buildCGIResponse" << std::endl;
    
    const CGIConfig& cgiConfig = config.getCgi();
    std::string requestPath = request.getPath();
    std::string fullPath = cgiConfig.root + requestPath;

    std::cout << "DEBUG: CGI Config - Root: " << cgiConfig.root << ", Path: " << cgiConfig.path << ", Extension: " << cgiConfig.ext << std::endl;
    std::cout << "DEBUG: Request Path: " << requestPath << std::endl;
    std::cout << "DEBUG: Full Path: " << fullPath << std::endl;

    // Check if the file exists
    if (access(fullPath.c_str(), F_OK) == -1) {
        std::cout << "DEBUG: CGI file not found at path: " << fullPath << std::endl;
        return buildErrorResponse(config, request, "404", "Not Found");
    }

    // Check if the file has the correct extension
    size_t dotPos = fullPath.find_last_of('.');
    if (dotPos == std::string::npos || fullPath.substr(dotPos) != cgiConfig.ext) {
        std::cout << "DEBUG: Invalid CGI file extension. Expected: " << cgiConfig.ext << ", Found: " << (dotPos != std::string::npos ? fullPath.substr(dotPos) : "no extension") << std::endl;
        return buildErrorResponse(config, request, "404", "Not Found");
    }

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        std::cerr << "ERROR: Failed to create pipe. Errno: " << errno << " - " << strerror(errno) << std::endl;
        return buildErrorResponse(config, request, "500", "Internal Server Error");
    }

    std::cout << "DEBUG: Forking process to execute CGI" << std::endl;
    pid_t pid = fork();
    if (pid == -1) {
        std::cerr << "ERROR: Fork failed. Errno: " << errno << " - " << strerror(errno) << std::endl;
        close(pipefd[0]);
        close(pipefd[1]);
        return buildErrorResponse(config, request, "500", "Internal Server Error");
    } else if (pid == 0) {
        // Child process
        std::cout << "DEBUG: Child process. Executing CGI script" << std::endl;
        close(pipefd[0]);  // Close read end of the pipe
        dup2(pipefd[1], STDOUT_FILENO);  // Redirect stdout to the pipe
        close(pipefd[1]);

        char* args[] = {const_cast<char*>(cgiConfig.path.c_str()), const_cast<char*>(fullPath.c_str()), NULL};
        execv(cgiConfig.path.c_str(), args);
        // If execv returns, it must have failed
        std::cerr << "ERROR: execv failed. Errno: " << errno << " - " << strerror(errno) << std::endl;
        exit(1);
    } else {
        // Parent process
        std::cout << "DEBUG: Parent process. Waiting for child to complete" << std::endl;
        close(pipefd[1]);  // Close write end of the pipe

        std::string cgiOutput;
        char buffer[4096];
        ssize_t bytesRead;
        while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
            cgiOutput.append(buffer, bytesRead);
        }
        close(pipefd[0]);

        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            std::cout << "DEBUG: CGI program executed successfully" << std::endl;
            std::cout << "DEBUG: CGI Output size: " << cgiOutput.length() << " bytes" << std::endl;

            RequestResponse response;
            response.setCgiContent(cgiOutput);
            std::cout << "DEBUG: CGI Output: " << std::endl;
            std::cout << "----------------------------------------" << std::endl;
            std::cout << cgiOutput << std::endl;
            std::cout << "----------------------------------------" << std::endl;
            return response;
        } else {
            std::cerr << "ERROR: CGI program failed. Exit status: " << WEXITSTATUS(status) << std::endl;
            return buildErrorResponse(config, request, "500", "Internal Server Error");
        }
    }
}
