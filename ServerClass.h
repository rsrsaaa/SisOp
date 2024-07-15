#ifndef SERVERCLASS_H
#define SERVERCLASS_H

#include <string>
#include <vector>
#include <mutex>
#include <unordered_map>
#include <netinet/in.h>
#include "global.hpp"

class Server
{
public:
    Server();
    int InitServerSocket();
    void ProcessDiscoveryMessage(const std::string &message);
    void AddClientToTable(const std::string &clientIP, int clientPort, const std::string &clientHostname);
    void ListenForDiscoveryMessages();
    void SendStatusUpdate(const std::string &clientIP, int clientPort, const std::string &statusMessage);
    void SendShutdownCommand(const std::string &clientHostname);
    int GetClientPort(const std::string &clientHostname);
    void Start();
    void SendWoLCommand(const std::string& hostname);
    void PrintClientTable();

private:
    std::vector<ClientInfo> clientTable;
    std::mutex tableMutex;
    int discoverSocket;
    int statusSocket;
    struct sockaddr_in discoverServerAddr;
};

#endif // SERVERCLASS_H
