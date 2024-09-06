/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bperez-a <bperez-a@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/06 09:57:15 by bperez-a          #+#    #+#             */
/*   Updated: 2024/09/06 10:08:35 by bperez-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestParser.hpp"


HttpRequest RequestParser::parse(const std::string& request_str) {
    HttpRequest request;
    std::istringstream iss(request_str);
    std::string method, path, protocol;
    std::getline(iss, method, ' ');
    std::getline(iss, path, ' ');
    std::getline(iss, protocol);
    request.setMethod(method);
    request.setPath(path);
    request.setProtocol(protocol);
    return request;
}
