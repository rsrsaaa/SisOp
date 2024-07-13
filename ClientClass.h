#ifndef CLIENTCLASS_H
#define CLIENTCLASS_H

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>

#define DISCOVERY_PORT_MIN 50000
#define DISCOVERY_PORT_MAX 50099
#define BUFFER_SIZE 256

class Client {
public:
    void run();
};

#endif // CLIENTCLASS_H
