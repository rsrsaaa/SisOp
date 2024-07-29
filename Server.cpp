#include "Server.hpp"
#include "global.hpp"
#include <iostream>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h> // Adicione para close()

using namespace std;

int Server::InitServerSocket() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Error opening socket");
        exit(1);
    }

    int reuse = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        perror("setsockopt(SO_REUSEADDR) failed");
        close(sockfd);
        exit(1);
    }

    int broadcastEnable = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0) {
        perror("Error setting socket option");
        close(sockfd);
        exit(1);
    }

    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(DISCOVER_PORT);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error binding socket");
        close(sockfd);
        exit(1);
    }
    return sockfd;
}

string Server::ListenToClientDiscover(int sockfd) {
    cout << "rodando" << endl;
    clilen = sizeof(cli_addr);
    unsigned int length;
    cout << "esperando dados do cliente" << endl;
    bzero(buff, sizeof(buff));
    length = sizeof(struct sockaddr_in);
    n = recvfrom(sockfd, buff, 256, 0, (struct sockaddr *) &cli_addr, &length);
    if (n < 0) {
        perror("Error reading from socket");
        exit(1);
    }
    return string(buff); // Corrija para retornar uma string
}

void Server::AddNewClientToTable() {
    cout << "adicionando novo cliente" << endl;
    for (int i = 0; i < 3; i++) {
        if (table[i].ip == " ") {
            // Simulação do MAC, pode precisar de uma forma correta de obter o MAC
            table[i].mac = "MAC_Address"; 
            table[i].name = inet_ntoa(cli_addr.sin_addr);
            table[i].port = ntohs(cli_addr.sin_port); // Corrija a conversão da porta
            table[i].ip = inet_ntoa(cli_addr.sin_addr);
            table[i].status = "AWAKEN";
            break;
        }
    }
}

void Server::SendStatusRequest() {
    for (int i = 0; i < 3; i++) {
        if (table[i].ip != " ") {
            int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
            if (sockfd < 0) {
                perror("Error opening socket");
                continue;
            }

            struct sockaddr_in dest_addr;
            memset(&dest_addr, 0, sizeof(dest_addr));
            dest_addr.sin_family = AF_INET;
            dest_addr.sin_port = htons(STATUS_PORT);
            dest_addr.sin_addr.s_addr = inet_addr(table[i].ip.c_str());

            const char *status_request = "sleep status request";
            n = sendto(sockfd, status_request, strlen(status_request), 0, (struct sockaddr*)&dest_addr, sizeof(dest_addr));
            if (n < 0) {
                perror("Error sending status request");
                close(sockfd);
                continue;
            }

            char buffer[256];
            struct timeval timeout;
            timeout.tv_sec = 5;
            timeout.tv_usec = 0;
            if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) < 0) {
                perror("setsockopt(SO_RCVTIMEO) failed");
            }

            struct sockaddr_in from;
            socklen_t fromlen = sizeof(from);
            n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&from, &fromlen);
            if (n < 0) {
                table[i].status = "ASLEEP";
            } else {
                table[i].status = "AWAKEN";
            }

            close(sockfd);
        }
    }
}

void Server::PrintTable() {
    cout << "Name\t\tIP\t\tMAC\t\tStatus\t\tPort" << endl;
    for (int i = 0; i < 3; i++) {
        if (table[i].ip != " ") {
            cout << table[i].name << "\t" << table[i].ip << "\t" << table[i].mac << "\t\t" << table[i].status << "\t\t" << table[i].port << endl;
        }
    }
}
