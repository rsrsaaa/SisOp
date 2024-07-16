#include "global.hpp"
#include "Management.hpp"
#include "Monitoring.hpp"
#include "Discovery.hpp"
#include "Interface.hpp"
#include <thread>
#include <iostream>

using namespace std;

pthread_mutex_t lock; // Definição do mutex
vector<managementTable> table(MAX_CLIENTS); // Definição da tabela de clientes

// Instâncias dos gerenciadores e subserviços
ClientManager clientManager;
MonitoringService monitoringService;
DiscoveryService discoveryService;
InterfaceService interfaceService;

// Função principal para inicialização e gerenciamento das threads
int main() {
    // Inicialização dos subserviços
    // Aqui você pode inicializar os dados iniciais, se necessário
    
    // Inicializar o mutex
    pthread_mutex_init(&lock, nullptr);

    // Threads para os subserviços com proteção mutex
    thread managementThread(&ClientManager::run, &clientManager);
    thread monitoringThread(&MonitoringService::run, &monitoringService);
    thread discoveryThread(&DiscoveryService::run, &discoveryService);
    thread interfaceThread(&InterfaceService::run, &interfaceService);

    // Espera até que todas as threads terminem
    managementThread.join();
    monitoringThread.join();
    discoveryThread.join();
    interfaceThread.join();

    // Destruir o mutex
    pthread_mutex_destroy(&lock);

    return 0;
}
