#include "ClientClass.h"

void Client::run() {
    int clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Failed to create socket." << std::endl;
        return;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(DISCOVERY_PORT_MIN);
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1"); // Change this IP as per your server's IP

    char message[] = "sleep discovery service";
    if (sendto(clientSocket, message, strlen(message), 0, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Failed to send message to server." << std::endl;
        close(clientSocket);
        return;
    }

    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));
    sockaddr_in serverResponse;
    socklen_t serverResponseSize = sizeof(serverResponse);
    if (recvfrom(clientSocket, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&serverResponse, &serverResponseSize) == -1) {
        std::cerr << "Failed to receive message from server." << std::endl;
        close(clientSocket);
        return;
    }

    std::cout << "Received message from server: " << buffer << std::endl;

    close(clientSocket);
}
