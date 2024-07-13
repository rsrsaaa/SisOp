#include "MonitoringSubservice.h"

void MonitoringSubservice::startMonitoring(bool isManager) {
    if (isManager) {
        thread monitoringThread(&MonitoringSubservice::sendStatusRequests, this);
        monitoringThread.detach();
    } else {
        thread monitoringThread(&MonitoringSubservice::receiveStatusRequests, this);
        monitoringThread.detach();
    }
}

void MonitoringSubservice::receiveStatusRequests() {
    int sockfd;
    struct sockaddr_in serverAddr, clientAddr;
    char buffer[BUFFER_SIZE];
    const int broadcastEnable = 1;

    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Error creating socket");
        exit(1);
    }

    // Enable broadcast option
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0) {
        perror("Error setting broadcast option");
        close(sockfd);
        exit(1);
    }

    // Configure server address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(STATUS_PORT_MIN);

    // Bind socket to address
    if (bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Error binding socket");
        close(sockfd);
        exit(1);
    }

    while (true) {
        socklen_t addrLen = sizeof(clientAddr);
        int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&clientAddr, &addrLen);
        if (n > 0) {
            buffer[n] = '\0';
            printf("Received status request: %s from %s\n", buffer, inet_ntoa(clientAddr.sin_addr));

            // Respond with current status
            const char* responseMessage = "awaken"; // Simulated status response
            if (sendto(sockfd, responseMessage, strlen(responseMessage), 0, (struct sockaddr*)&clientAddr, addrLen) < 0) {
                perror("Error sending status response");
            }
        }
    }

    // Close socket
    close(sockfd);
}

void MonitoringSubservice::sendStatusRequests() {
    int sockfd;
    struct sockaddr_in broadcastAddr;
    const char* broadcastIP = "192.168.1.255"; // Example broadcast IP
    const int broadcastEnable = 1;
    char message[] = STATUS_REQUEST_MESSAGE;

    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Error creating socket");
        exit(1);
    }

    // Enable broadcast option
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0) {
        perror("Error setting broadcast option");
        close(sockfd);
        exit(1);
    }

    // Configure the broadcast address
    memset(&broadcastAddr, 0, sizeof(broadcastAddr));
    broadcastAddr.sin_family = AF_INET;
    broadcastAddr.sin_port = htons(STATUS_PORT_MIN);
    broadcastAddr.sin_addr.s_addr = inet_addr(broadcastIP);

    // Broadcast status request message
    while (true) {
        if (sendto(sockfd, message, strlen(message), 0, (struct sockaddr*)&broadcastAddr, sizeof(broadcastAddr)) < 0) {
            perror("Error sending status request");
        }
        sleep(10); // Adjust as needed for the status check interval
    }

    // Close socket
    close(sockfd);
}
