#include "ServerClass.h"
#include "ClientClass.h"
#include "global.hpp"
#include <pthread.h>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <signal.h>

pthread_mutex_t lock;

static void *ThreadServerDiscover(void *arg)
{
    Server *server = static_cast<Server *>(arg);
    while (isRunning) // Verifica se o servidor ainda está em execução
    {
        server->ListenForDiscoveryMessages();
        pthread_mutex_lock(&lock);
        server->PrintClientTable(); // Corrigido para PrintClientTable
        pthread_mutex_unlock(&lock);

        std::this_thread::sleep_for(std::chrono::seconds(5)); // Intervalo de 5 segundos entre impressões
    }
    return NULL;
}

static void *ThreadServerCommand(void *arg)
{
    Server *server = static_cast<Server *>(arg);
    while (isRunning) // Verifica se o servidor ainda está em execução
    {
        std::string command;
        std::getline(std::cin, command);

        if (command.substr(0, 7) == "WAKEUP ")
        {
            std::string hostname = command.substr(7);
            server->SendWoLCommand(hostname); // Chama SendWoLCommand na classe Server
        }
        else if (command == "EXIT")
        {
            isRunning = false;
            break;
        }
    }
    return NULL;
}

static void *ThreadClient(void *arg)
{
    Client *client = static_cast<Client *>(arg);
    client->SendDiscoveryMessages();

    return NULL;
}

void handleSignal(int signal)
{
    if (signal == SIGINT)
    {
        std::cerr << "Ctrl+C pressed. Forcing exit." << std::endl;
        exit(EXIT_FAILURE); // Força a saída imediata do programa
    }
}

int main(int argc, char *argv[])
{
    signal(SIGINT, handleSignal); // Configura o tratamento do sinal SIGINT (Ctrl+C)

    if (argc < 1 || argc > 2)
    {
        std::cerr << "Usage: " << argv[0] << " [manager]" << std::endl;
        return 1;
    }

    std::string role = (argc == 2) ? argv[1] : "client";

    if (role == "manager")
    {
        Server server;
        server.InitServerSocket();
        pthread_t tidDiscover, tidCommand;
        pthread_create(&tidDiscover, NULL, ThreadServerDiscover, &server);
        pthread_create(&tidCommand, NULL, ThreadServerCommand, &server);

        pthread_join(tidDiscover, NULL);
        pthread_join(tidCommand, NULL);
    }
    else if (role == "client")
    {
        Client client;
        client.InitDiscoverSocket();
        pthread_t tidClient;
        pthread_create(&tidClient, NULL, ThreadClient, &client);

        pthread_join(tidClient, NULL);
    }
    else
    {
        std::cerr << "Invalid role: " << role << std::endl;
        return 1;
    }

    return 0;
}
