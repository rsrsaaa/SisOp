// global.hpp

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <mutex>

using namespace std;

#define DISCOVER_PORT 53000
#define STATUS_PORT 52000
#define MAX_MESSAGE_SIZE 1024
#define MAX_CLIENTS 10
extern int clientNum;

struct managementTable
{
    string name;
    string ip;
    string mac;
    string status;
    int port;
};

class ClientManager {
private:
    vector<managementTable> clientTable;
    mutex tableMutex;

public:
    ClientManager();

    void addClient(const managementTable& client);
    void removeClient(const string& ip);
    void updateClientStatus(const string& ip, const string& newStatus);
    vector<managementTable> getClientTable();
    void refreshClientStatus();
    bool isClientAwake(const string &ip);
    string pingClient(const string &ip);
};

extern pthread_mutex_t lock; // Declaração do mutex

// Função para limpar a tela, dependendo do sistema operacional
void clearScreen();
