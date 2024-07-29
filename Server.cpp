#include "Server.hpp"
#include "global.hpp"
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <thread>
#include <chrono>

using namespace std;

// Inicializa o socket do servidor
int Server::InitServerSocket() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Error opening socket");
        exit(1);
    }

    int broadcastEnable = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0) {
        perror("Error setting socket option");
        exit(1);
    }

    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(DISCOVER_PORT);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error binding socket");
        exit(1);
    }

    return sockfd;
}

// Escuta mensagens de descoberta do cliente
string Server::ListenToClientDiscover(int sockfd) {
    cout << "rodando" << endl;
    clilen = sizeof(cli_addr);
    bzero(buff, sizeof(buff));
    
    int n = recvfrom(sockfd, buff, sizeof(buff), 0, (struct sockaddr *) &cli_addr, &clilen);
    if (n < 0) {
        perror("Error reading from socket");
        exit(1);
    }
    
    return string(buff);
}

// Adiciona um novo cliente à tabela
void Server::AddNewClientToTable() {
    cout << "adicionando novo cliente" << endl;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (table[i].ip.empty()) {
            table[i].mac = inet_ntoa(cli_addr.sin_addr); // Note: you might need to change how you store MAC addresses
            table[i].name = inet_ntoa(cli_addr.sin_addr);
            table[i].port = ntohs(cli_addr.sin_port);
            table[i].ip = inet_ntoa(cli_addr.sin_addr);
            table[i].status = "AWAKEN";
            break;
        }
    }
}

// Envia uma solicitação de status para todos os clientes
void Server::SendStatusRequest() {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (!table[i].ip.empty()) {
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
            int n = sendto(sockfd, status_request, strlen(status_request), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
            if (n < 0) {
                perror("Error sending status request");
                close(sockfd);
                continue;
            }

            char buffer[256];
            struct timeval timeout;
            timeout.tv_sec = 5;
            timeout.tv_usec = 0;
            setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout));

            struct sockaddr_in from;
            socklen_t fromlen = sizeof(from);
            n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&from, &fromlen);
            if (n < 0) {
                table[i].status = "ASLEEP";
            } else {
                table[i].status = "AWAKEN";
            }

            close(sockfd);
        }
    }
}

// Imprime a tabela de clientes
void Server::PrintTable() {
    cout << "Name\t\tIP\t\tMAC\t\tStatus\t\tPort" << endl;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (!table[i].ip.empty()) {
            cout << table[i].name << "\t" << table[i].ip << "\t" << table[i].mac << "\t\t" << table[i].status << "\t\t" << table[i].port << endl;
        }
    }
}

// Inicia o serviço de descoberta
void Server::StartDiscovery() {
    while (true) {
        string message = ListenToClientDiscover(sockfd);
        if (!message.empty()) {
            AddNewClientToTable();
            SendStatusRequest();
            PrintTable();
        }
    }
}

// Atualiza o status dos clientes
void Server::StartMonitoring() {
    while (true) {
        SendStatusRequest(); // Atualiza o status de todos os clientes
        PrintTable(); // Atualiza a tela com a tabela de clientes
        this_thread::sleep_for(chrono::seconds(5));
    }
}

// Inicializa e gerencia todos os serviços
void Server::StartManagement() {
    thread discoveryThread(&Server::StartDiscovery, this);
    thread monitoringThread(&Server::StartMonitoring, this);

    discoveryThread.join();
    monitoringThread.join();
}
