/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bperez-a <bperez-a@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 20:11:38 by bperez-a          #+#    #+#             */
/*   Updated: 2024/09/11 11:09:40 by bperez-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <string>
#include <map>
#include <iostream>

class HttpRequest {
public:
    HttpRequest();
    HttpRequest(const HttpRequest& other);
    HttpRequest& operator=(const HttpRequest& other);
    ~HttpRequest();

    std::string getMethod() const;
    std::string getPath() const;
    std::string getProtocol() const;
    std::string getBody() const;
    std::string getHeader(const std::string& key) const;
    const std::map<std::string, std::string>& getHeaders() const;

    void setMethod(const std::string& method);
    void setPath(const std::string& path);
    void setProtocol(const std::string& protocol);
    void setBody(const std::string& body);
    void addHeader(const std::string& key, const std::string& value);

    bool hasHeader(const std::string& key) const;
    void printHeaders() const;

private:
    std::string method_;
    std::string path_;
    std::string protocol_;
    std::map<std::string, std::string> headers_;
    std::string body_;
};

std::ostream& operator<<(std::ostream& os, const HttpRequest& request);

#endif // HTTPREQUEST_HPP
