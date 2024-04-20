#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main() {
    int serverSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrSize = sizeof(clientAddr);
    char buffer[1024];

    // Create UDP socket
    serverSocket = socket(PF_INET, SOCK_DGRAM, 0);
    if (serverSocket < 0) {
        std::cerr << "Error in creating socket.\n";
        return 1;
    }

    if(setsockopt(serverSocket, SOL_SOCKET, SO_BROADCAST, &(int){1}, sizeof(int)) < 0) {
        std::cerr << "Error in setsockopt.\n";
        return 1;
    }
    // Server address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(53000); // Port number
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); // Bind to any available IP address

    // Bind socket to address and port
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Error in binding.\n";
        return 1;
    }

    // Receive data from client
    ssize_t recvSize = recvfrom(serverSocket, buffer, sizeof(buffer), 0, (struct sockaddr*)&clientAddr, &addrSize);
    if (recvSize < 0) {
        std::cerr << "Error in receiving data.\n";
        return 1;
    }

    // Print received data
    std::cout << "Received message from client: " << buffer << std::endl;

    close(serverSocket);
    return 0;
}