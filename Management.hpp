#ifndef MANAGEMENT_HPP
#define MANAGEMENT_HPP

#include "global.hpp"
#include <vector>
#include <mutex>

// Estrutura para informações de gerenciamento de clientes
struct managementTable {
    std::string name;
    std::string ip;
    std::string mac;
    std::string status;
    int port;
};

// Classe para gerenciamento de clientes
class ClientManager {
public:
    ClientManager(); // Construtor padrão

    void addClient(const managementTable& client); // Adicionar cliente à tabela
    void removeClient(const std::string& ip); // Remover cliente da tabela
    void updateClientStatus(const std::string& ip, const std::string& newStatus); // Atualizar status do cliente
    std::vector<managementTable> getClientTable(); // Obter tabela de clientes

    void refreshClientStatus(); // Atualizar status de todos os clientes na tabela usando ping
    bool isClientAwake(const std::string& ip); // Verificar se um cliente está acordado usando ping

private:
    std::vector<managementTable> clientTable; // Tabela de clientes
    std::mutex tableMutex; // Mutex para proteção da tabela
};

#endif // MANAGEMENT_HPP
