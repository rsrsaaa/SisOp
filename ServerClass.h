#ifndef SERVERCLASS_H
#define SERVERCLASS_H

#include <string>
#include <vector>
#include <mutex>
#include "global.hpp"


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

private:
    std::vector<ClientInfo> clientTable;
    std::mutex tableMutex;
    int discoverSocket; // Socket UDP para comunicação de descoberta
    int statusSocket;   // Socket UDP para comunicação de status
};

#endif // SERVERCLASS_H
