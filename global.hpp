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

// Definição da estrutura managementTable
struct managementTable
{
    string name;
    string ip;
    string mac;
    string status;
    int port;
};

// Declarações externas
extern vector<managementTable> table; // Declaração externa
extern int clientNum; // Declaração externa
extern pthread_mutex_t global_lock; // Declaração externa

// Definição da classe ClientManager
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

// Função para limpar a tela, dependendo do sistema operacional
void clearScreen();
