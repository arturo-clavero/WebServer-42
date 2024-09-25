/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestResponse.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bperez-a <bperez-a@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/06 10:16:00 by bperez-a          #+#    #+#             */
/*   Updated: 2024/09/25 12:15:22 by bperez-a         ###   ########.fr       */
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
	const std::vector<int> getPostFileFds() const;
	const std::vector<std::string> getPostFileContents() const;
	std::string getCgiContent() const;
	//Setters
	void setBody(const std::string& body);
	void setStatusCode(const std::string& statusCode);
	void setStatusMessage(const std::string& statusMessage);
	void setContentType(const std::string& contentType);
	void setContentLength(size_t contentLength);
	void setContentDisposition(const std::string& contentDisposition);
	void setFilePathForBody(const std::string& file_path_for_body);
	void setContentLengthFromPath(const std::string& path);
	void setPostFileContents(const std::vector<std::string> & contents);
	void setPostFileFds(const std::vector<int> & fds);
	void setCgiContent(const std::string& cgiContent);
	
	void popBackPostFileContents();
	void popBackPostFileFds();

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
	std::vector<std::string> postFileContents;
	std::vector<int> postFileFds;

	std::string cgiContent;

};

std::ostream& operator<<(std::ostream& os, const RequestResponse& requestResponse);

#endif
