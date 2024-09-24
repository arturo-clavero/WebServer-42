/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseUtils.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bperez-a <bperez-a@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/09 09:40:51 by bperez-a          #+#    #+#             */
/*   Updated: 2024/09/24 18:45:45 by bperez-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_UTILS_HPP
#define RESPONSE_UTILS_HPP

#include "includes.hpp"
#include "RequestResponse.hpp"
#include "../request_parser/HttpRequest.hpp"
#include "../config/ServerConfig.hpp"


enum FileType {
	IS_FILE,
	IS_DIRECTORY,
	NOT_FOUND
};

class ResponseUtils {
public:
	static bool isRequestValid(const HttpRequest& request);
	static bool isRequestTooLarge(const HttpRequest& request, const size_t& clientMaxBodySize);
	static LocationConfig findLocation(const std::string& path, const ServerConfig& config);
	static bool isMethodAllowed(const HttpRequest& request, const LocationConfig& location);
	static FileType getTargetType(const HttpRequest& request);
	static std::string getContentType(const std::string& path);	
	static std::string getContentDisposition(const std::string& path);
	static bool saveFile(const std::string& path, const std::string& filename, const std::string& content);
	
private:
    ResponseUtils();
};

#endif
