#include "ServerClass.h"

void Server::start() {
    int sockfd;
    struct sockaddr_in servAddr, clientAddr;
    char buffer[BUFFER_SIZE];

    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Error creating socket");
        return;
    }

    // Configure server address
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = INADDR_ANY;     // Pode ser usado para qualquer endereço IP da máquina
    servAddr.sin_port = htons(DISCOVERY_PORT_MIN); // Porta do servidor

    // Bind socket to address
    if (bind(sockfd, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0) {
        perror("Error binding socket");
        close(sockfd);
        return;
    }

    while (true) {
        socklen_t addrLen = sizeof(clientAddr);
        int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&clientAddr, &addrLen);
        if (n > 0) {
            buffer[n] = '\0';
            printf("Received request: %s from %s\n", buffer, inet_ntoa(clientAddr.sin_addr));

            // Simulate response
            const char *responseMessage = "awaken";
            if (sendto(sockfd, responseMessage, strlen(responseMessage), 0, (struct sockaddr *)&clientAddr, addrLen) < 0) {
                perror("Error sending response");
            }
        }
    }

    // Close socket
    close(sockfd);
}
