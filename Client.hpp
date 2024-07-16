#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "global.hpp"
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

class Client {
public:
    int clientSocket, clientStatusSocket, n;
    struct sockaddr_in serverAddress;
    int myClientNum = 0;

    void InitClientSocket();
    void SendRequestToServer();
    void ListenToServer();
};

#endif // CLIENT_HPP
