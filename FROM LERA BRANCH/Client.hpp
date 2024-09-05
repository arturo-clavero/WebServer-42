#ifndef CLIENT_HPP
#define CLIENT_HPP

// class Response;
// class Request; // add class implement from request 


class Client {
    public:
        int clientSocket;                 // Socket descriptor for this client
        struct sockaddr_in clientAddr;    // Client's address information
        std::string ipAddress;            // Client's IP address
        int port;                         // Client's port number

    // public:
    //     Client(int clientSocket, struct sockaddr_in clientAddr); // Constructor
    //     ~Client();                                                // Destructor

    //     ssize_t readData(char* buffer, size_t bufferSize);   // Read data from client
    //     ssize_t writeData(const char* data, size_t dataSize);  // Write data to client

    //     int getClientSocket() const;           // Get client's socket descriptor
    //     struct sockaddr_in getStruct() const;    // Get client's address structure

    //     Response response;   // Response object for this client
    //     Request request;      // Request object from this client

    //     void setPort(int port);          // Set client's port
    //     void setIp(std::string ip);   // Set client's IP address
    //     std::string getIp() const;      // Get client's IP address
    //     int getPort() const;         // Get client's port

};

#endif