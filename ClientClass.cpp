#include "ClientClass.h"
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include "global.hpp"

Client::Client() {
    discoverSocket = 0;
    statusSocket = 0;
}

void Client::InitClientSocket() {
    // Inicializa o socket UDP para descoberta
    if ((discoverSocket = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        std::cerr << "Failed to create discover client socket." << std::endl;
        exit(EXIT_FAILURE);
    }

    // Configura o endereço do servidor para descoberta
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(DISCOVER_PORT); // Porta de descoberta
    serverAddr.sin_addr.s_addr = INADDR_ANY;    // Poderia ser substituído pelo endereço IP do servidor

    // Inicializa o socket UDP para status
    if ((statusSocket = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        std::cerr << "Failed to create status client socket." << std::endl;
        exit(EXIT_FAILURE);
    }

    // Configura o endereço do servidor para status
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(STATUS_PORT); // Porta de status
    serverAddr.sin_addr.s_addr = INADDR_ANY;   // Poderia ser substituído pelo endereço IP do servidor
}

void Client::SendDiscoveryMessage() {
    std::string discoveryMessage = "sleep service discovery";
    std::lock_guard<std::mutex> guard(socketMutex); // Garante exclusão mútua

    ssize_t sentBytes = sendto(discoverSocket, discoveryMessage.c_str(), discoveryMessage.length(), 0,
                               (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if (sentBytes == -1) {
        std::cerr << "Failed to send discovery message." << std::endl;
    } else {
        std::cout << "Discovery message sent to server." << std::endl;
    }
}

void Client::ListenForStatusUpdates() {
    char buffer[1024] = {0};
    struct sockaddr_in serverAddr;
    socklen_t serverAddrLen = sizeof(serverAddr);
    ssize_t numBytes;

    bool shouldExit = false;  // Flag para indicar quando sair do loop

    while (!shouldExit) {
        {
            std::lock_guard<std::mutex> guard(socketMutex); // Garante exclusão mútua

            // Recebe atualizações de status do servidor
            numBytes = recvfrom(statusSocket, buffer, sizeof(buffer), 0, (struct sockaddr *)&serverAddr, &serverAddrLen);
        }

        if (numBytes == -1) {
            std::cerr << "Error receiving status update from server." << std::endl;
        } else {
            std::cout << "Received status update from server: " << buffer << std::endl;

            // Processa a mensagem de atualização de status recebida
            std::string message(buffer);
            // Verifica se a mensagem indica que o cliente deve sair
            if (message == "EXIT") {
                shouldExit = true;
                std::cout << "Exiting status update listener..." << std::endl;
                // Aqui você pode adicionar código para realizar outras operações necessárias antes de sair do loop
                // Por exemplo, enviar uma confirmação ao servidor de que o cliente está saindo
            }
        }
    }

    // Após sair do loop, aqui você pode realizar outras operações de limpeza, como fechar sockets
    close(statusSocket);  // Fecha o socket de status
    // Outras operações de limpeza, se necessário
}

