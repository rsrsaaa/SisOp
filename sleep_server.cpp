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

void runDiscovery(Server &server) {
    // Lógica de inicialização e execução do serviço de descoberta
    while (true) {
        string message = server.ListenToClientDiscover(server.InitServerSocket());
        // Processa a mensagem recebida e lida com a descoberta de clientes
    }
}

void runManagement(Server &server) {
    // Lógica de inicialização e execução do serviço de gerenciamento
    while (true) {
        server.SendStatusRequest();
        server.PrintTable();
        std::this_thread::sleep_for(std::chrono::seconds(10)); // Intervalo de tempo entre verificações de status
    }
}

void runMonitoring(Server &server) {
    // Lógica de inicialização e execução do serviço de monitoramento
    while (true) {
        // Implementação do monitoramento dos clientes conectados
    }
}

void runInterface(Server &server) {
    // Lógica de inicialização e execução da interface de comando
    while (true) {
        // Interface para receber comandos de entrada do administrador
    }
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        // Modo cliente
        Client client;
        client.InitClientSocket();
        client.SendRequestToServer();
        client.ListenToServer();
    } else if (argc == 2 && std::string(argv[1]) == "manager") {
        // Modo servidor (manager)
        Server server;
        
        // Cria e inicializa threads para cada subserviço
        std::thread discoveryThread(runDiscovery, std::ref(server));
        std::thread managementThread(runManagement, std::ref(server));
        std::thread monitoringThread(runMonitoring, std::ref(server));
        std::thread interfaceThread(runInterface, std::ref(server));

        // Juntar as threads para esperar que todas terminem (embora neste caso todas sejam loops infinitos)
        discoveryThread.join();
        managementThread.join();
        monitoringThread.join();
        interfaceThread.join();
    } else {
        std::cerr << "Uso: " << argv[0] << " [manager]" << std::endl;
        return 1;
    }

    return 0;
}
