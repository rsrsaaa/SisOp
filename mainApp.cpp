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
    Server server;
    int socket = server.InitServerSocket();
    while (1)
    {
        server.ListenToClientDiscover();
        pthread_mutex_lock(&lock);
        server.PrintTable(); // Atualiza a tabela na tela do servidor
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

static void *ThreadServerCommand(void *arg)
{
    Server server;
    int socket = server.InitServerSocket();
    while (1)
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

int main(int argc, char *argv[])
{
    pthread_t tid[2];
    pthread_create(&tid[0], NULL, ThreadServerDiscover, NULL);
    pthread_create(&tid[1], NULL, ThreadServerCommand, NULL);
    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);

    return 0;
}
