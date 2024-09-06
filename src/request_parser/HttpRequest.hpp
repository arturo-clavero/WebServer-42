/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bperez-a <bperez-a@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 20:11:38 by bperez-a          #+#    #+#             */
/*   Updated: 2024/09/06 15:22:21 by bperez-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include "includes.hpp"

class HttpRequest {
public:
    HttpRequest();
    HttpRequest(const HttpRequest& other);
    HttpRequest& operator=(const HttpRequest& other);
    ~HttpRequest();

    std::string getMethod() const;
    std::string getPath() const;
    std::string getProtocol() const;
	std::string getType() const;

	void setMethod(const std::string& method);
	void setPath(const std::string& path);
	void setProtocol(const std::string& protocol);
	void setType(const std::string& type);
	
private:
    std::string method;
    std::string path;
    std::string protocol;
	std::string type;
};

//overload << operator to print the request
std::ostream& operator<<(std::ostream& os, const HttpRequest& request);

#endif
