/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artclave <artclave@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 20:13:14 by bperez-a          #+#    #+#             */
/*   Updated: 2024/09/12 10:49:14 by artclave         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

HttpRequest::HttpRequest() {}

HttpRequest::HttpRequest(const HttpRequest& other)
    : method_(other.method_),
      path_(other.path_),
      protocol_(other.protocol_),
      headers_(other.headers_),
      body_(other.body_),
	  host_(other.host_) {}

HttpRequest& HttpRequest::operator=(const HttpRequest& other) {
    if (this != &other) {
        method_ = other.method_;
        path_ = other.path_;
        protocol_ = other.protocol_;
        headers_ = other.headers_;
        body_ = other.body_;
		host_ = other.host_;
    }
    return *this;
}

HttpRequest::~HttpRequest() {}

std::string HttpRequest::getMethod() const { return method_; }
std::string HttpRequest::getPath() const { return path_; }
std::string HttpRequest::getProtocol() const { return protocol_; }
std::string HttpRequest::getBody() const { return body_; }
std::string HttpRequest::getHost() const { return host_; } //for network version 1


std::string HttpRequest::getHeader(const std::string& key) const {
    std::map<std::string, std::string>::const_iterator it = headers_.find(key);
    return (it != headers_.end()) ? it->second : "";
}

const std::map<std::string, std::string>& HttpRequest::getHeaders() const {
    return headers_;
}

void HttpRequest::setMethod(const std::string& method) { method_ = method; }
void HttpRequest::setPath(const std::string& path) { path_ = path; }
void HttpRequest::setProtocol(const std::string& protocol) { protocol_ = protocol; }
void HttpRequest::setBody(const std::string& body) { body_ = body; }
void HttpRequest::setHost(const std::string& host) { host_ = host; } //for network version 1


void HttpRequest::addHeader(const std::string& key, const std::string& value) {
    if (!key.empty()) {
        headers_[key] = value;
    }
}

bool HttpRequest::hasHeader(const std::string& key) const {
    return headers_.find(key) != headers_.end();
}

void HttpRequest::printHeaders() const {
    std::cout << "Headers:" << std::endl;
    std::map<std::string, std::string>::const_iterator it;
    for (it = headers_.begin(); it != headers_.end(); ++it) {
        std::cout << "  " << it->first << ": " << it->second << std::endl;
    }
}

std::ostream& operator<<(std::ostream& os, const HttpRequest& request) {
    os << "Method: " << request.getMethod() << "\n"
       << "Path: " << request.getPath() << "\n"
       << "Protocol: " << request.getProtocol() << "\n";
    request.printHeaders();
    os << "Body: " << request.getBody() << "\n";
	os << "Host: " << request.getHost() << "\n"; //for network version 1
    return os;
}