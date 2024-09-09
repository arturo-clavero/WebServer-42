/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestResponse.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bperez-a <bperez-a@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/06 10:17:15 by bperez-a          #+#    #+#             */
/*   Updated: 2024/09/09 14:43:45 by bperez-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestResponse.hpp"

RequestResponse::RequestResponse() {
}

RequestResponse::~RequestResponse() {
}

std::string RequestResponse::getBody() const { return this->body; }
std::string RequestResponse::getStatusCode() const { return this->statusCode; }
std::string RequestResponse::getStatusMessage() const { return this->statusMessage; }
std::string RequestResponse::getContentType() const { return this->contentType; }
size_t RequestResponse::getContentLength() const { return this->contentLength; }
std::string RequestResponse::getContentDisposition() const { return this->contentDisposition; }

void RequestResponse::setBody(const std::string& body) { this->body = body; }
void RequestResponse::setStatusCode(const std::string& statusCode) { this->statusCode = statusCode; }
void RequestResponse::setStatusMessage(const std::string& statusMessage) { this->statusMessage = statusMessage; }
void RequestResponse::setContentType(const std::string& contentType) { this->contentType = contentType; }
void RequestResponse::setContentLength(size_t contentLength) { this->contentLength = contentLength; }
void RequestResponse::setContentDisposition(const std::string& contentDisposition) { this->contentDisposition = contentDisposition; }

std::string RequestResponse::toString() const {
	std::string response;	
	response += "HTTP/1.1 " + this->statusCode + " " + this->statusMessage + "\r\n";
	response += "Content-Type: " + this->contentType + "\r\n";
	
	std::stringstream ss;
	ss << this->contentLength;
	std::string contentLengthStr = ss.str();
	
	response += "Content-Length: " + contentLengthStr + "\r\n";
	if (!this->contentDisposition.empty())
		response += "Content-Disposition: " + this->contentDisposition + "\r\n";
	response += "\r\n";
	response += this->body;
	return response;
}

std::ostream& operator<<(std::ostream& os, const RequestResponse& requestResponse) {
	//prints everything but the body
	os << "HTTP/1.1 " << requestResponse.getStatusCode() << " " << requestResponse.getStatusMessage() << "\r\n";
	os << "Content-Type: " << requestResponse.getContentType() << "\r\n";
	os << "Content-Length: " << requestResponse.getContentLength() << "\r\n";
	os << "Content-Disposition: " << requestResponse.getContentDisposition() << "\r\n";
	
	os << "\r\n";
	return os;
}
