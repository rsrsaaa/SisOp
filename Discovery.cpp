#include "Discovery.hpp"
#include "global.hpp"
#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <mutex>

using namespace std;

extern pthread_mutex_t lock;          // Mutex externo definido em global.hpp
extern vector<managementTable> table; // Tabela de clientes definida em global.hpp

void *ThreadDiscovery(void *arg)
{
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len;
    char buffer[MAX_MESSAGE_SIZE];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1)
    {
        perror("Erro ao criar socket");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(DISCOVER_PORT);

    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
    {
        perror("Erro ao realizar bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    while (true)
    {
        len = sizeof(cliaddr);
        int n = recvfrom(sockfd, buffer, MAX_MESSAGE_SIZE, MSG_WAITALL, (struct sockaddr *)&cliaddr, &len);
        if (n == -1)
        {
            perror("Erro ao receber mensagem");
            continue;
        }

        buffer[n] = '\0';

        // Processa a mensagem recebida
        DiscoveryMessage *msg = (DiscoveryMessage *)buffer;
        if (strcmp(msg->messageType, "sleep discovery service") == 0)
        {
            pthread_mutex_lock(&lock); // Adiciona o endereço do mutex para bloquear
            // Atualiza a tabela de clientes
            for (int i = 0; i < MAX_CLIENTS; ++i)
            {
                if (table[i].ip.empty())
                {
                    table[i].ip = msg->clientIP;
                    table[i].mac = msg->clientMAC;
                    table[i].port = msg->clientPort;
                    table[i].status = "AWAKEN";
                    break;
                }
            }
            pthread_mutex_unlock(&lock); // Adiciona o endereço do mutex para desbloquear
        }
    }

    close(sockfd);
    return NULL;
}
