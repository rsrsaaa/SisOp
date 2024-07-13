// range de portas 50000 - 54999
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#define PORT 53000

class Client {
public:
    void run() {
        // range de portas 50000 - 54999
        int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket == -1) {
            std::cerr << "Failed to create socket." << std::endl;
            return;
        }

        sockaddr_in serverAddress;
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(PORT);
        serverAddress.sin_addr.s_addr = inet_addr("255.255.255.255");

        if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
            std::cerr << "Failed to connect to server." << std::endl;
            close(clientSocket);
            return;
        }

        char message[] = "Hello, server!";
        if (send(clientSocket, message, strlen(message), 0) == -1) {
            std::cerr << "Failed to send message to server." << std::endl;
            close(clientSocket); 
            return;
        }

        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        if (recv(clientSocket, buffer, sizeof(buffer), 0) == -1) {
            std::cerr << "Failed to receive message from server." << std::endl;
            close(clientSocket);
            return;
        }

        std::cout << "Received message from server: " << buffer << std::endl;

        close(clientSocket);
    }
};
