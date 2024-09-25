/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostRequestBodySnatcher.cpp                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bperez-a <bperez-a@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 22:14:57 by bperez-a          #+#    #+#             */
/*   Updated: 2024/09/25 16:03:57 by bperez-a         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PostRequestBodySnatcher.hpp"


std::vector<PostRequestBodyPart> PostRequestBodySnatcher::parse(const std::string& body, const std::string& boundary) {
	std::vector<PostRequestBodyPart> parts;
	std::istringstream bodyStream(body);
	std::string line;
	PostRequestBodyPart currentPart;
	bool inHeaders = false;
	bool inContent = false;

	std::cout << "DEBUG: entering body parse for Post request" << std::endl;

	while (std::getline(bodyStream, line)) {
		if (line.find(boundary) != std::string::npos) {
			if (inContent) {
				parts.push_back(currentPart);
				currentPart = PostRequestBodyPart();
			}
			inHeaders = true;
			inContent = false;
		} else if (inHeaders && line == "\r") {
			inHeaders = false;
			inContent = true;
		} else if (inHeaders) {
			// Parse headers
			size_t colonPos = line.find(':');
			if (colonPos != std::string::npos) {
				std::string key = line.substr(0, colonPos);
				std::string value = line.substr(colonPos + 2); // Skip ": "
				if (key == "Content-Disposition") {
					size_t namePos = value.find("name=\"");
					if (namePos != std::string::npos) {
						size_t nameEnd = value.find("\"", namePos + 6);
						currentPart.setName(value.substr(namePos + 6, nameEnd - (namePos + 6)));
					}
					size_t filenamePos = value.find("filename=\"");
					if (filenamePos != std::string::npos) {
						size_t filenameEnd = value.find("\"", filenamePos + 10);
						currentPart.setFilename(value.substr(filenamePos + 10, filenameEnd - (filenamePos + 10)));
					}
				} else if (key == "Content-Type") {
					currentPart.setContentType(value);
				}
			}
		} else if (inContent) {
			// Append content
			currentPart.appendContent(line + "\n");
		}
	}

	if (inContent) {
		parts.push_back(currentPart);
	}

	std::cout << "DEBUG: request body parsed " << std::endl;
	return parts;
}