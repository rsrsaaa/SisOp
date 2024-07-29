#include "Client.hpp"
#include "global.hpp"
#include <iostream>
#include <cstdlib>    // Para system()
#include <cstring>    // Para memset, strlen
#include <unistd.h>   // Para close

using namespace std;

// Inicializa o socket do cliente
void Client::InitClientSocket() {
    cout << "rodando cliente" << endl;
    clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket == -1) {
        cerr << "Failed to create client socket." << endl;
        exit(EXIT_FAILURE);
    }

    int broadcastEnable = 1;
    if (setsockopt(clientSocket, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0) {
        cerr << "Error setting socket option" << endl;
        close(clientSocket);
        exit(EXIT_FAILURE);
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(DISCOVER_PORT);
    serverAddress.sin_addr.s_addr = inet_addr("255.255.255.255");
}

// Envia uma solicitação para o servidor
void Client::SendRequestToServer() {
    cout << "enviando mensagem para o servidor" << endl;
    const char *message = "sleep discovery service";
    int n = sendto(clientSocket, message, strlen(message), 0, (const struct sockaddr *) &serverAddress, sizeof(serverAddress));
    if (n < 0) {
        cerr << "Failed to send message to server." << endl;
        close(clientSocket);
        exit(EXIT_FAILURE);
    }
}

// Escuta as respostas do servidor
void Client::ListenToServer() {
    int clientStatusSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientStatusSocket == -1) {
        cerr << "Failed to create client status socket." << endl;
        close(clientSocket);
        exit(EXIT_FAILURE);
    }

    cout << "Listening for server connections on the status port." << endl;
    struct sockaddr_in statusAddress;
    socklen_t servlen = sizeof(struct sockaddr_in);
    memset(&statusAddress, 0, sizeof(statusAddress));
    
    statusAddress.sin_family = AF_INET;
    statusAddress.sin_port = htons(STATUS_PORT);
    statusAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(clientStatusSocket, (struct sockaddr*)&statusAddress, sizeof(statusAddress)) == -1) {
        cerr << "Failed to bind status socket." << endl;
        close(clientStatusSocket);
        close(clientSocket);
        exit(EXIT_FAILURE);
    }

    char message[1024];
    while (true) {
        ssize_t n = recvfrom(clientStatusSocket, message, sizeof(message) - 1, 0, (struct sockaddr *) &statusAddress, &servlen);
        if (n < 0) {
            cerr << "ERROR on recvfrom" << endl;
            continue;
        }
        message[n] = '\0';  // Adiciona o terminador nulo à string
        cout << "Received a datagram: " << message << endl;

        n = sendto(clientStatusSocket, "ack", strlen("ack"), 0, (struct sockaddr *) &statusAddress, sizeof(statusAddress));
        if (n < 0) {
            cerr << "ERROR on sendto" << endl;
        }
    }

    close(clientStatusSocket);
    close(clientSocket);
}
