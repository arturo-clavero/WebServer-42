/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostRequestBodyPart.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: artclave <artclave@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 22:07:49 by bperez-a          #+#    #+#             */
/*   Updated: 2024/10/07 05:07:53 by artclave         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef POST_REQUEST_BODY_PART_HPP
# define POST_REQUEST_BODY_PART_HPP

#include "../includes.hpp"

class PostRequestBodyPart {
public:
    PostRequestBodyPart();

    std::string getName() const;
    std::string getFilename() const;
    std::string getContentType() const;
    std::string getContent() const;

    void setName(const std::string& name);
    void setFilename(const std::string& filename);
    void setContentType(const std::string& content_type);
    void setContent(const std::string& content);

    void appendContent(const std::string& content);

private:
    std::string name_;
    std::string filename_;
    std::string content_type_;
    std::string content_;
};

#endif
