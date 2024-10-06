/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostRequestBodySnatcher.hpp                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artclave <artclave@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 22:14:07 by bperez-a          #+#    #+#             */
/*   Updated: 2024/10/07 05:07:49 by artclave         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef POST_REQUEST_BODY_SNATCHER_HPP
# define POST_REQUEST_BODY_SNATCHER_HPP

#include "PostRequestBodyPart.hpp"
#include "../includes.hpp"

class PostRequestBodySnatcher {
private:
    PostRequestBodySnatcher();    

public:
    static std::vector<PostRequestBodyPart> parse(const std::string& body, const std::string& boundary);
};

#endif
