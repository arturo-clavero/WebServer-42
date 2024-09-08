#include "ServerSocket.hpp"

ServerSocket::ServerSocket() : listenSocket(-1), backlog(32), port(0){}
ServerSocket::~ServerSocket() {}
int ServerSocket::getPort() const {return port;}
std::string ServerSocket::getIp() const {return ipAddress;}

int ServerSocket::getListenSocket() const {return listenSocket;}

void  ServerSocket::initListenSocket(std::string ipAddress, int port)
{
    this->ipAddress = ipAddress;
    this->port = port;
    listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == -1){
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }
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
     // Set port in network byte order
    serverAddr.sin_port = htons(port);
    int opt = 1;
    // Set socket option to reuse address
    setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Bind the socket to the specified IP and port
    if (bind(listenSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1){
        perror("Error bindong socket");
        close(listenSocket);
        exit (EXIT_FAILURE);
    }
  // Start listening for incoming connections
    if (listen(listenSocket, backlog) == -1){
        perror ("Error listening on socket");
        close(listenSocket);
        exit(EXIT_FAILURE);
    }
}
