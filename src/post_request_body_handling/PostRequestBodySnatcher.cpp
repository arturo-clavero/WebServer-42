/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostRequestBodySnatcher.cpp                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bperez-a <bperez-a@student.42bangkok.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 22:14:57 by bperez-a          #+#    #+#             */
/*   Updated: 2024/09/12 22:43:38 by bperez-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PostRequestBodySnatcher.hpp"


std::vector<PostRequestBodyPart> PostRequestBodySnatcher::parse(const std::string& body, const std::string& boundary) {
	std::vector<PostRequestBodyPart> parts;

    std::string::size_type start = 0;
    std::string::size_type end = 0;
    std::cout << "DEBUG: entering body parse for Post request" << std::endl;
    while ((end = body.find(boundary, start)) != std::string::npos) {
        PostRequestBodyPart part;
        std::string partContent = body.substr(start, end - start);
        
        // Parse the part content to set the PostRequestBodyPart object's properties
        size_t headerEnd = partContent.find("\r\n\r\n");
        if (headerEnd != std::string::npos) {
            std::string headers = partContent.substr(0, headerEnd);
            std::string content = partContent.substr(headerEnd + 4);
            
            // Parse headers to set name, filename, and content type
            size_t namePos = headers.find("name=\"");
            if (namePos != std::string::npos) {
                size_t nameEnd = headers.find("\"", namePos + 6);
                part.setName(headers.substr(namePos + 6, nameEnd - (namePos + 6)));
            }
            
            size_t filenamePos = headers.find("filename=\"");
            if (filenamePos != std::string::npos) {
                size_t filenameEnd = headers.find("\"", filenamePos + 10);
                part.setFilename(headers.substr(filenamePos + 10, filenameEnd - (filenamePos + 10)));
            }
            
            size_t contentTypePos = headers.find("Content-Type: ");
            if (contentTypePos != std::string::npos) {
                size_t contentTypeEnd = headers.find("\r\n", contentTypePos + 14);
                part.setContentType(headers.substr(contentTypePos + 14, contentTypeEnd - (contentTypePos + 14)));
            }
            std::cout << "DEBUG: Part: " << part.getName() << " " << part.getFilename() << " " << part.getContentType() << std::endl;
            part.setContent(content);
        }
        parts.push_back(part);
        start = end + boundary.length();
    }
	
	return parts;
}