#include <iostream>
#include <cstring>     // For memset
#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <arpa/inet.h>  // For inet_addr
#include <unistd.h>     // For close()

int main(int ach, char **argv) {
    const char* server_ip = "127.0.0.1";  // Local server IP address
    const int server_port = 8080;          // Server port number
    const char* message = \
"POST /upload HTTP/1.1\r\nHost: example.com\r\nSec-Fetch-Dest: image\r\nContent-Type: text/plain\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n4\r\nW0iki\r\n5\r\npedia\r\nE\r\n in chunks.\r\n0\r\n\r\n"
;
(void)ach;
(void)argv;
    // Create a socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Failed to create socket\n";
        return 1;
    }

    // Specify server address
    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr)); // Clear the structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr.s_addr = inet_addr(server_ip);

    // Connect to the server
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Connection failed\n";
        close(sock);
        return 1;
    }

    std::cout << "Connected to server\n";

    // Send a message to the server
    if (send(sock, message, strlen(message), 0) < 0) {
        std::cerr << "Failed to send message\n";
        close(sock);
        return 1;
    }

    std::cout << "Message sent to server\n";

    // Receive a response from the server
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    int bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received < 0) {
        std::cerr << "Failed to receive message\n";
    } else {
        std::cout << "Received from server: " << buffer << std::endl;
    }
	std::cout<<message;
	while (1)
		{}
    // Close the socket
    close(sock);
    return 0;
}