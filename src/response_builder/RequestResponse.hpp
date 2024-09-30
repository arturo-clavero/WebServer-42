/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestResponse.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artclave <artclave@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/06 10:16:00 by bperez-a          #+#    #+#             */
/*   Updated: 2024/09/30 18:24:45 by artclave         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_RESPONSE_HPP
#define REQUEST_RESPONSE_HPP

#include "includes.hpp"
#include "../config/ServerConfig.hpp"



class RequestResponse {
public:
    RequestResponse();
    ~RequestResponse();

	//Getters
	std::string getBody() const;
	std::string getStatusCode() const;
	std::string getStatusMessage() const;
	std::string getContentType() const;	
	std::string getContentDisposition() const;
	size_t getContentLength() const;
	std::string getFilePathForBody() const;
	std::string getCgiPath() const;
	//Setters
	void setBody(const std::string& body);
	void setStatusCode(const std::string& statusCode);
	void setStatusMessage(const std::string& statusMessage);
	void setContentType(const std::string& contentType);
	void setContentLength(size_t contentLength);
	void setContentDisposition(const std::string& contentDisposition);
	void setFilePathForBody(const std::string& file_path_for_body);
	void setContentLengthFromPath(const std::string& path);
	void setCgiPath(const std::string& cgiPath);
	

	bool buildBodyFromFile(const ServerConfig& config, int file_fd);
    std::string toString() const;

private:
	std::string body;
	std::string file_path_for_body;
	std::string statusCode;
	std::string statusMessage;
	std::string contentType;
	std::string contentDisposition;
	size_t contentLength;

	std::string cgi_path;

};

std::ostream& operator<<(std::ostream& os, const RequestResponse& requestResponse);

#endif
