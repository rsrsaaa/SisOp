#ifndef CLIENTCLASS_H
#define CLIENTCLASS_H

#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <mutex>

class Client
{
public:
    Client();
    void InitClientSocket();
    void SendDiscoveryMessage();
    void ListenForStatusUpdates();
    void SendStatusUpdate();
    void ProcessCommand(const std::string &command);
    void WakeUpStation(const std::string &hostname);
    void SendWoLCommand(const std::string &hostname);
    void SendUDPMessage(const std::string &ip, int port, const std::string &message);

private:
    int discoverSocket;
    int statusSocket;
    std::string serverIP;
    int serverPort;
    bool asleep;
    struct sockaddr_in serverAddrDiscover; // Declaração para descoberta
    struct sockaddr_in serverAddrStatus;   // Declaração para status
    std::mutex socketMutex;
};

#endif // CLIENTCLASS_H
