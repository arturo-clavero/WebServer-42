/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestResponse.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bperez-a <bperez-a@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/06 10:16:00 by bperez-a          #+#    #+#             */
/*   Updated: 2024/09/09 11:29:41 by bperez-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_RESPONSE_HPP
#define REQUEST_RESPONSE_HPP

#include "includes.hpp"


class RequestResponse {
public:
    RequestResponse();
    ~RequestResponse();

	//Getters
	std::string getBody() const;
	std::string getStatusCode() const;
	std::string getStatusMessage() const;
	std::string getContentType() const;
	size_t getContentLength() const;
	std::string getType() const;

	//Setters
	void setBody(const std::string& body);
	void setStatusCode(const std::string& statusCode);
	void setStatusMessage(const std::string& statusMessage);
	void setContentType(const std::string& contentType);
	void setContentLength(size_t contentLength);
	void setType(const std::string& type);

    std::string toString() const;

private:
	std::string body;
	std::string statusCode;
	std::string statusMessage;
	std::string contentType;
	size_t contentLength;

};

std::ostream& operator<<(std::ostream& os, const RequestResponse& requestResponse);

#endif
