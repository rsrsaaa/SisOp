#ifndef SERVER_HPP
#define SERVER_HPP

#include "global.hpp"
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

class Server {
private:
    int sockfd;  // Adiciona sockfd como membro da classe
    managementTable table[3]; // Definição de 'table' como um array de managementTable

public:
    int newsockfd, n;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    char buff[256];

    int InitServerSocket();
    std::string ListenToClientDiscover(int sockfd);
    void AddNewClientToTable();
    void SendStatusRequest();
    void PrintTable();

    void StartDiscovery();
    void StartMonitoring();
    void StartManagement();
};

#endif // SERVER_HPP
