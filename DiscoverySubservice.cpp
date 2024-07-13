#include "DiscoverySubservice.h"

vector<Participant> DiscoverySubservice::getParticipants() {
    return participants;
}

void DiscoverySubservice::startDiscovery(bool isManager) {
    if (isManager) {
        thread discoveryThread(&DiscoverySubservice::receiveDiscovery, this);
        discoveryThread.detach();
    } else {
        thread discoveryThread(&DiscoverySubservice::sendDiscovery, this);
        discoveryThread.detach();
    }
}

void DiscoverySubservice::receiveDiscovery() {
    int sockfd;
    struct sockaddr_in serverAddr, clientAddr;
    char buffer[BUFFER_SIZE];

    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Error creating socket");
        exit(1);
    }

    // Configure server address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(DISCOVERY_PORT_MIN);

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
            printf("Received discovery message: %s from %s\n", buffer, inet_ntoa(clientAddr.sin_addr));

            // Add participant to the list
            Participant newParticipant;
            strcpy(newParticipant.ip, inet_ntoa(clientAddr.sin_addr));
            newParticipant.port = ntohs(clientAddr.sin_port);
            newParticipant.asleep = false; // Assuming awake initially
            participants.push_back(newParticipant);
        }
    }

    // Close socket
    close(sockfd);
}

void DiscoverySubservice::sendDiscovery() {
    int sockfd;
    struct sockaddr_in broadcastAddr;
    const char* broadcastIP = "192.168.1.255"; // Example broadcast IP
    const int broadcastEnable = 1;
    char message[] = DISCOVERY_MESSAGE;

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
    broadcastAddr.sin_port = htons(DISCOVERY_PORT_MIN);
    broadcastAddr.sin_addr.s_addr = inet_addr(broadcastIP);

    // Broadcast discovery message
    while (true) {
        if (sendto(sockfd, message, strlen(message), 0, (struct sockaddr*)&broadcastAddr, sizeof(broadcastAddr)) < 0) {
            perror("Error sending broadcast message");
        }
        sleep(5); // Adjust as needed for the discovery interval
    }

    // Close socket
    close(sockfd);
}
