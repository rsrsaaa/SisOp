#include <iostream>
#include <thread>
#include <vector>
#include <cstring>
#include <mutex>
#include "Discovery.hpp"
#include "Management.hpp"
#include "Monitoring.hpp"
#include "Interface.hpp"
#include "global.hpp"
#include "Server.hpp"
#include "Client.hpp"

using namespace std;

// Função para iniciar o serviço como Manager (Servidor)
void startServer() {
    Server server;
    int sockfd = server.InitServerSocket();

    while (true) {
        string message = server.ListenToClientDiscover(sockfd);
        if (!message.empty()) {
            server.AddNewClientToTable();
            server.SendStatusRequest();
            server.PrintTable();
        }
    }
}

// Função para iniciar o serviço como Participant (Cliente)
void startClient() {
    Client client;
    client.InitClientSocket();
    client.SendRequestToServer();

    thread listenThread(&Client::ListenToServer, &client);
    listenThread.detach();

    // Para não finalizar imediatamente, enquanto os threads estão rodando
    while (true) {
        this_thread::sleep_for(chrono::seconds(10));
    }
}

// Função para iniciar todos os serviços
void startAllServices(bool isManager) {
    if (isManager) {
        // Iniciar o serviço como Manager
        thread serverThread(startServer);
        serverThread.detach();
    } else {
        // Iniciar o serviço como Participant
        thread clientThread(startClient);
        clientThread.detach();

        // Iniciar o serviço de monitoramento
        thread monitoringThread(startMonitoring);
        monitoringThread.detach();

        // Iniciar a interface de usuário
        thread interfaceThread(showInterface);
        interfaceThread.detach();
    }
}

int main(int argc, char *argv[]) {
    bool isManager = false;

    if (argc > 1 && strcmp(argv[1], "manager") == 0) {
        isManager = true;
    }

    startAllServices(isManager);

    // Manter o programa rodando
    while (true) {
        this_thread::sleep_for(chrono::seconds(60));
    }

    return 0;
}
