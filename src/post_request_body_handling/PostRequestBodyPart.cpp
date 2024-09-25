/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostRequestBodyPart.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bperez-a <bperez-a@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 22:13:22 by bperez-a          #+#    #+#             */
/*   Updated: 2024/09/25 16:05:19 by bperez-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PostRequestBodyPart.hpp"

PostRequestBodyPart::PostRequestBodyPart() {}

std::string PostRequestBodyPart::getName() const { return name_; }
std::string PostRequestBodyPart::getFilename() const { return filename_; }
std::string PostRequestBodyPart::getContentType() const { return content_type_; }
std::string PostRequestBodyPart::getContent() const { return content_; }

void PostRequestBodyPart::setName(const std::string& name) { name_ = name; }
void PostRequestBodyPart::setFilename(const std::string& filename) { filename_ = filename; }
void PostRequestBodyPart::setContentType(const std::string& content_type) { content_type_ = content_type; }
void PostRequestBodyPart::setContent(const std::string& content) { content_ = content; }

void PostRequestBodyPart::appendContent(const std::string& content) {
	content_ += content;
}