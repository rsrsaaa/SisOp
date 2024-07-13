#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <iostream>
#include <arpa/inet.h>
// #include <arpa/inet.h>
// range de portas 50000 - 54999
#define PORT 53000
using namespace std;
struct managementTable
{
    char name[20] = " ";
    char ip[20] = " ";
    int port = 0;
};
void clearscreen()
{
#ifdef _WIN32
        system("cls");

#elif _POSIX_C_SOURCE >= 199309L
        system("clear");

#endif
}

class Server 
{

public:
    int sockfd, n;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;
	char buff[256];
    managementTable table[3];
    void startServer() 
    {
        // Create socket
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            perror("Error opening socket");
            exit(1);
        }

        // Initialize server address
        memset((char *) &serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(PORT);

        // Bind socket to address
        if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
            perror("Error binding socket");
            exit(1);
        }
        int running = 1;
        while(running)
        {
            
            // Listen for connections
            cout<<"rodando"<<endl;
            listen(sockfd, 5);
            clilen = sizeof(cli_addr);

            // Enable broadcast
            int broadcastEnable = 1;
            if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0) {
                perror("Error setting socket option");
                exit(1);
            }
            // Accept incoming connections
            int newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
            if (newsockfd < 0) {
                perror("Error accepting connection");
                exit(1);
            }

            // Read data from client
            cout<<"esperando dados do cliente"<<endl;
            bzero(buff, sizeof(buff));
            n = read(newsockfd, buff, sizeof(buff));
            if (n < 0) {
                perror("Error reading from socket");
                exit(1);
            }
            clearscreen();
            if(strcmp(buff,"sleep discovery service") == 0)
            {
                cout<<"adicionando novo cliente"<<endl;
                for (int i = 0; i < 3; i++)
                {
                    if(strcmp(table[i].ip," ") == 0)
                    {
                        strcpy(table[i].name, inet_ntoa(cli_addr.sin_addr));
                        table[i].port = cli_addr.sin_port;
                        strcpy(table[i].ip, inet_ntoa(cli_addr.sin_addr));
                        break;
                    }
                }
            }
            // Process data
            printf("Received message: %s\n", buff);
            char message[] = "Hello, client!";
            if (send(newsockfd, message, strlen(message), 0) == -1) {
                cerr << "Failed to send message to client." << endl;
                close(newsockfd);
                return;
            }
            close(newsockfd);
            
            cout << "Tabela de gerenciamento de servicos" << endl;
            for (int i = 0; i < 3; i++)
            {
                cout << "Cliente: " << table[i].name << " IP: " << table[i].ip << " Porta: " << table[i].port << endl;
            }
        }

        // Close sockets
        close(sockfd);
    }
};

