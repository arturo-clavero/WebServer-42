/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseBuilder.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bperez-a <bperez-a@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/06 10:15:00 by bperez-a          #+#    #+#             */
/*   Updated: 2024/09/06 10:19:47 by bperez-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_BUILDER_HPP
#define RESPONSE_BUILDER_HPP

#include "includes.hpp"
#include "../request_parser/HttpRequest.hpp"
#include "../config/ServerConfig.hpp"
#include "RequestResponse.hpp"

class ResponseBuilder {
public:
    static RequestResponse build(const HttpRequest& request, const ServerConfig& config);

private:
    ResponseBuilder() {}
};

#endif

