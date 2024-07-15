#ifndef SERVERCLASS_H
#define SERVERCLASS_H

#include <string>
#include <vector>
#include <mutex>
#include "global.hpp"
#include <string>
#include <unordered_map>



struct ClientInfo
{
    std::string hostname;
    std::string macAddress;
    std::string ipAddress;
    std::string status;

    friend std::ostream &operator<<(std::ostream &os, const ClientInfo &client);
};

class Server
{
public:
    Server();
    int InitDiscoverSocket();
    void UpdateClientTable(const std::string &hostname, const std::string &ip, const std::string &mac, const std::string &status);
    int InitServerSocket();
    void ListenToClientDiscover();
    void ProcessDiscoveryMessage(const std::string &message);
    void SendStatusRequest();
    void PrintTable();
    void UpdateClientStatus(const std::string &hostname, const std::string &status);
    void AddNewClientToTable(const ClientInfo &client);
    void AddClientToTable(struct sockaddr_in clientAddr);
    void PrintClientTable();

private:
    std::vector<ClientInfo> clientTable;
    std::mutex tableMutex;
    int discoverSocket; // Socket UDP para comunicação de descoberta
    int statusSocket;   // Socket UDP para comunicação de status
    struct sockaddr_in discoverServerAddr;
    struct sockaddr_in statusServerAddr;
    std::unordered_map<std::string, int> clientsTable;  // Tabela de clientes
    std::mutex clientTableMutex;  // Mutex para proteção da tabela de clientes
};

#endif // SERVERCLASS_H
