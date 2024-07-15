#ifndef CLIENTCLASS_H
#define CLIENTCLASS_H

#include <string>
#include <netinet/in.h>
#include <mutex>

class Client
{
public:
    Client();
    void InitClientSocket();
    void SendDiscoveryMessage();
    void ListenForStatusUpdates();

private:
    int discoverSocket; // Socket UDP para descoberta
    int statusSocket;   // Socket UDP para status
    struct sockaddr_in serverAddr;
    std::mutex socketMutex;
};

#endif // CLIENTCLASS_H
