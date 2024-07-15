#include "ServerClass.h"
#include "ClientClass.h"
#include <pthread.h>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include "global.hpp"

pthread_mutex_t lock;

static void *ThreadServerDiscover(void *arg)
{
    Server *server = static_cast<Server *>(arg);
    while (true)
    {
        server->ListenToClientDiscover();
        pthread_mutex_lock(&lock);
        server->PrintTable(); // Atualiza a tabela na tela do servidor
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

static void *ThreadServerCommand(void *arg)
{
    Server *server = static_cast<Server *>(arg);
    while (true)
    {
        std::string command;
        std::getline(std::cin, command);

        if (command.substr(0, 7) == "WAKEUP ")
        {
            std::string hostname = command.substr(7);
            // Implementar lógica para enviar comando WAKEUP para o cliente específico (usando WoL)
        }
        else if (command == "EXIT")
        {
            // Implementar lógica para encerrar o servidor de forma segura, se necessário
            break;
        }
    }
    return NULL;
}

static void *ThreadClient(void *arg)
{
    Client client;
    client.InitClientSocket();
    client.SendDiscoveryMessage();

    pthread_t tidStatus;
    pthread_create(&tidStatus, NULL, [](void *arg) -> void *
                   {
        Client *client = static_cast<Client*>(arg);
        client->ListenForStatusUpdates();
        return NULL; }, &client);

    while (true)
    {
        std::string command;
        std::getline(std::cin, command);
        client.ProcessCommand(command);
    }

    pthread_join(tidStatus, NULL);
    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <mode>" << std::endl;
        std::cerr << "Modes: 'manager' for server, 'client' for client" << std::endl;
        return 1;
    }

    std::string mode = argv[1];

    if (mode == "manager")
    {
        Server server;
        if (server.InitServerSocket() == -1)
        {
            return 1;
        }

        pthread_t tid[2];
        pthread_create(&tid[0], NULL, ThreadServerDiscover, &server);
        pthread_create(&tid[1], NULL, ThreadServerCommand, &server);
        pthread_join(tid[0], NULL);
        pthread_join(tid[1], NULL);
    }
    else if (mode == "client")
    {
        pthread_t tid;
        pthread_create(&tid, NULL, ThreadClient, NULL);
        pthread_join(tid, NULL);
    }
    else
    {
        std::cerr << "Invalid mode. Use 'manager' for server, 'client' for client." << std::endl;
        return 1;
    }

    return 0;
}
