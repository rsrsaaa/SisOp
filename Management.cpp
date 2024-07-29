#include "Management.hpp"
#include <iostream>
#include <cstdlib>    // Para system()
#include <sstream>    // Para stringstream

ClientManager clientManager; // Definição do clientManager

ClientManager::ClientManager() {}

// Adicionar um novo cliente à tabela
void ClientManager::addClient(const managementTable& client) {
    lock_guard<mutex> lock(tableMutex);
    clientTable.push_back(client);
}

// Remover um cliente da tabela com base no IP
void ClientManager::removeClient(const string& ip) {
    lock_guard<mutex> lock(tableMutex);
    for (auto it = clientTable.begin(); it != clientTable.end(); ++it) {
        if (it->ip == ip) {
            clientTable.erase(it);
            break;
        }
    }
}

// Atualizar o status de um cliente na tabela com base no IP
void ClientManager::updateClientStatus(const string& ip, const string& newStatus) {
    lock_guard<mutex> lock(tableMutex);
    for (auto& client : clientTable) {
        if (client.ip == ip) {
            client.status = newStatus;
            break;
        }
    }
}

// Obter a tabela de clientes atual
vector<managementTable> ClientManager::getClientTable() {
    lock_guard<mutex> lock(tableMutex);
    return clientTable;
}

// Atualiza o status de todos os clientes na tabela usando ping
void ClientManager::refreshClientStatus() {
    for (auto& client : clientTable) {
        bool isAwake = isClientAwake(client.ip);
        if (isAwake) {
            client.status = "AWAKEN";
        } else {
            client.status = "ASLEEP";
        }
    }
}

// Função para verificar se um cliente está acordado usando ping
bool ClientManager::isClientAwake(const string& ip) {
    stringstream pingCommand;
    pingCommand << "ping -c 1 " << ip;

    // Executa o comando ping e captura o resultado
    if (system(pingCommand.str().c_str()) == 0) {
        return true;  // Cliente respondeu ao ping, está acordado
    } else {
        return false; // Cliente não respondeu ao ping, está dormindo
    }
}