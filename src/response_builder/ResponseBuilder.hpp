/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseBuilder.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artclave <artclave@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/06 10:15:00 by bperez-a          #+#    #+#             */
/*   Updated: 2024/10/02 19:56:18 by artclave         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_BUILDER_HPP
#define RESPONSE_BUILDER_HPP

#include "includes.hpp"
#include "../request_parser/HttpRequest.hpp"
#include "../config/ServerConfig.hpp"
#include "../post_request_body_handling/PostRequestBodySnatcher.hpp"
#include "../post_request_body_handling/PostRequestBodyPart.hpp"
#include "RequestResponse.hpp"

class ResponseBuilder {
public:

    static RequestResponse build(HttpRequest& request, ServerConfig& config);
	
	static RequestResponse buildGetResponse(ServerConfig& config, HttpRequest& request, LocationConfig& location);
	static RequestResponse buildGetFileResponse(ServerConfig& config, HttpRequest& request, LocationConfig& location);
	static RequestResponse buildGetDirectoryResponse(ServerConfig& config, HttpRequest& request, LocationConfig& location);

	static RequestResponse buildPostResponse(ServerConfig& config, HttpRequest& request, LocationConfig& location);
	static RequestResponse buildPostSuccessResponse(ServerConfig& config, HttpRequest& request, LocationConfig& location);
	static bool processPostData(HttpRequest& request, std::string& path);
	
	static RequestResponse buildDeleteResponse(ServerConfig& config, HttpRequest& request, LocationConfig& location);

	static RequestResponse buildErrorResponse(ServerConfig& config, HttpRequest& request, const std::string& code, const std::string& message);
	static RequestResponse buildSuccessResponse(ServerConfig& config, HttpRequest& request, LocationConfig& location, std::string& path);
	static RequestResponse buildAutoindexResponse(ServerConfig& config, HttpRequest& request, LocationConfig& location, std::string& path);

	static RequestResponse buildCGIResponse(ServerConfig& config, HttpRequest& request);


private:
    ResponseBuilder() {}
};

#endif

