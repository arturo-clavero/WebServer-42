#ifndef SERVER_HPP
#define SERVER_HPP

#include "Libraries.hpp"
#include "Parser.hpp"
#include "Location.hpp"

class Server : public Parser {
    private:
        std::vector<std::string> serverConfig;  // Stores server confoguration lines
        std::string ipAddress;                  // Server's IP addrss
        int port;                              // Server's port number
        int clientMaxBodySize;                // Maximum allowed size for client request body
        std::string serverName;               // Name of the serer
        std::map<int, std::string> errorPages // Custom error pages (status code -> page path)
        std::vector<Location> locations;        // Server's location blocks

    public:
        Server();  // Constructor

        void setConfig(std::string &configLine);  // Adds a config line to serverConfig
        void addDataInServer();                // Processes serverConfig to set up the server
        void printServerData();                // Prints server configuration (for debugging)
        void printDataConfig() const;          // Prints raw conlig data (for debugging)

        // Getter methods
        int getPort() const;
        const std::string &getHost() const;
        const std::string &getServerName() const;
        std::map<int, std::string> getErrorPages() const;
        const std::vector<Location> &getLocations() const;
        int getClientMaxBodySize() const;

    Server &operator=(Server const &src);  // Assignment operator
};



#endif