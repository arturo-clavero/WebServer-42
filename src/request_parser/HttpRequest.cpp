/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bperez-a <bperez-a@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 20:13:14 by bperez-a          #+#    #+#             */
/*   Updated: 2024/09/05 14:02:06 by bperez-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

HttpRequest::HttpRequest() { }

HttpRequest::HttpRequest(const HttpRequest& other)
    : method(other.method),
      path(other.path),
      protocol(other.protocol) { }

HttpRequest& HttpRequest::operator=(const HttpRequest& other) {
    if (this != &other) {
        this->method = other.method;
        this->path = other.path;
        this->protocol = other.protocol;
    }
    return *this;
}

HttpRequest::~HttpRequest() { }

std::string HttpRequest::getMethod() const {
    return this->method;
}

std::string HttpRequest::getPath() const {
    return this->path;
}

std::string HttpRequest::getProtocol() const {
    return this->protocol;
}

void HttpRequest::parse(const std::string& request) {
    std::istringstream iss(request);
    std::getline(iss, this->method, ' ');
    std::getline(iss, this->path, ' ');
    std::getline(iss, this->protocol);
}


//overload << operator to print the request
std::ostream& operator<<(std::ostream& os, const HttpRequest& request) {
    os << "Method: " << request.getMethod() << std::endl;
    os << "Path: " << request.getPath() << std::endl;
    os << "Protocol: " << request.getProtocol() << std::endl;
    return os;
}