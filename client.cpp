#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;
    socklen_t addrSize;

    // Create UDP socket
    clientSocket = socket(PF_INET, SOCK_DGRAM, 0);
    if (clientSocket < 0) {
        std::cerr << "Error in creating socket.\n";
        return 1;
    }

    if(setsockopt(clientSocket, SOL_SOCKET, SO_BROADCAST, &(int){1}, sizeof(int)) < 0) {
        std::cerr << "Error in setsockopt.\n";
        return 1;
    }
    // Server address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(53000); // Port number
    serverAddr.sin_addr.s_addr = inet_addr("255.255.255.255"); // Server IP address

    std::string message = "Hello from UDP client";
    // Send data to server
    sendto(clientSocket, message.c_str(), message.length(), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

    close(clientSocket);
    return 0;
}