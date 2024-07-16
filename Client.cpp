#include "Client.hpp"
#include "global.hpp"
#include <iostream>
#include <cstdlib>    // Para system()
#include <sstream>    // Para stringstream
#include <fstream>    // Para ifstream
#include <stdexcept>  // Para runtime_error

using namespace std;

void Client::InitClientSocket() {
    cout << "rodando cliente" << endl;
    clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket == -1) {
        cerr << "Failed to create client socket." << endl;
        return;
    }
    
    int broadcastEnable = 1;
    if (setsockopt(clientSocket, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0) {
        cerr << "Error setting socket option" << endl;
        return;
    }
    
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(DISCOVER_PORT);
    serverAddress.sin_addr.s_addr = inet_addr("255.255.255.255");
}

void Client::SendRequestToServer() {
    cout << "enviando mensagem para o servidor" << endl;
    char message[] = "sleep discovery service";
    n = sendto(clientSocket, message, strlen(message), 0, (const struct sockaddr *) &serverAddress, sizeof(struct sockaddr_in));
    if (n < 0) {
        cerr << "Failed to send message to server." << endl;
        return;
    }
    myClientNum = clientNum;
    clientNum++;
}

void Client::ListenToServer() {
    clientStatusSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientStatusSocket == -1) {
        cerr << "Failed to create client status socket." << endl;
        return;
    }

    cout << "Listening for server connections on the status port." << endl;
    struct sockaddr_in statusAddress, serverAddress;
    socklen_t servlen;
    statusAddress.sin_family = AF_INET;
    statusAddress.sin_port = htons(STATUS_PORT);
    statusAddress.sin_addr.s_addr = INADDR_ANY;
    servlen = sizeof(struct sockaddr_in);

    if (bind(clientStatusSocket, (struct sockaddr*)&statusAddress, sizeof(statusAddress)) == -1) {
        cerr << "Failed to bind status socket." << endl;
        close(clientStatusSocket);
        return;
    }

    char message[1024];
    unsigned int length;
    length = sizeof(struct sockaddr_in);
    while(1) {
        n = recvfrom(clientStatusSocket, message, 256, 0, (struct sockaddr *) &serverAddress, &servlen);
        if (n < 0) {
            cerr << "ERROR on recvfrom" << endl;
            continue;
        }
        cout << "Received a datagram: " << message << endl;
        
        n = sendto(clientStatusSocket, "ack", sizeof("ack"), 0, (struct sockaddr *) &serverAddress, sizeof(struct sockaddr));
        if (n < 0) {
            cerr << "ERROR on sendto" << endl;
        }
    }
}
