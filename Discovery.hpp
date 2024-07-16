#ifndef DISCOVERY_HPP
#define DISCOVERY_HPP

#include "global.hpp"
#include <pthread.h>

// Função para realizar a descoberta de novos clientes via UDP
void *ThreadDiscovery(void *arg);

struct DiscoveryMessage {
    char messageType[50];
    char clientIP[50];
    char clientMAC[50];
    int clientPort;
    // Outros campos conforme necessário
};

#endif // DISCOVERY_HPP
