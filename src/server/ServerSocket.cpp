#include "ServerSocket.hpp"

ServerSocket::ServerSocket() : listenSocket(-1), port(0), backlog(32){}
ServerSocket::~ServerSocket() {}
int ServerSocket::getPort() const {return port;}
std::string ServerSocket::getIp() const {return ipAddress;}

int ServerSocket::getListenSocket() const {return listenSocket;}

void  ServerSocket::initListenSocket(std::string ipAddress, int port)
{
    std::cout << "Initializing listen socket with IP: " << ipAddress << " and port: " << port << std::endl;
    this->ipAddress = ipAddress;
    this->port = port;
    listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == -1){
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }
    std::cout << "Socket created successfully with fd: " << listenSocket << std::endl;
    memset(&serverAddr, 0, sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;

    if (inet_pton(AF_INET, ipAddress.c_str(), &(serverAddr.sin_addr)) <= 0) {
            /*The binary representation needs to be in network 
            byte order (big-endian)and the  sockaddr_in structure (which serverAddr is) 
            requires the IP address in its binary, network 
            byte order form.*/
        perror ("Error of ip number");
        exit (EXIT_FAILURE);
    }
    std::cout << "IP address converted successfully" << std::endl;
     // Set port in network byte order
    serverAddr.sin_port = htons(port);
    int opt = 1;
    // Set socket option to reuse address
    if (setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("Error setting socket option");
        close(listenSocket);
        exit(EXIT_FAILURE);
    }
    std::cout << "Socket option set successfully" << std::endl;

    // Bind the socket to the specified IP and port
    if (bind(listenSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1){
        perror("Error binding socket");
        close(listenSocket);
        exit (EXIT_FAILURE);
    }
    std::cout << "Socket bound successfully" << std::endl;
  // Start listening for incoming connections
    if (listen(listenSocket, backlog) == -1){
        perror ("Error listening on socket");
        close(listenSocket);
        exit(EXIT_FAILURE);
    }
    std::cout << "Socket now listening with backlog: " << backlog << std::endl;
}
