#ifndef SERVERCLASS_H
#define SERVERCLASS_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <iostream>

#define DISCOVERY_PORT_MIN 50000
#define DISCOVERY_PORT_MAX 50099
#define BUFFER_SIZE 256

class Server {
public:
    void start();
};

#endif // SERVERCLASS_H
