#ifndef MONITORING_HPP
#define MONITORING_HPP

#include "global.hpp"
#include <iostream>
#include <mutex>

extern ClientManager clientManager; // Referência externa para o gerenciador de clientes definido em Management.cpp

// Função para realizar o monitoramento do status dos clientes via UDP
void startMonitoring();

#endif // MONITORING_HPP
