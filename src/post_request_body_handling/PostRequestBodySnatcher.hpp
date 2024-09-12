/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostRequestBodySnatcher.hpp                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bperez-a <bperez-a@student.42bangkok.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 22:14:07 by bperez-a          #+#    #+#             */
/*   Updated: 2024/09/12 22:27:15 by bperez-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef POST_REQUEST_BODY_SNATCHER_HPP
# define POST_REQUEST_BODY_SNATCHER_HPP

#include "PostRequestBodyPart.hpp"
#include "includes.hpp"

class PostRequestBodySnatcher {
private:
    PostRequestBodySnatcher();    

public:
    static std::vector<PostRequestBodyPart> parse(const std::string& body, const std::string& boundary);
};

#endif
