#include "global.hpp"
// #include <arpa/inet.h>
class Server 
{
public:
    int newsockfd, n;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;
	char buff[256];


    int InitServerSocket()
    {
        // Create socket
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            perror("Error opening socket");
            exit(1);
        }
        int broadcastEnable = 1;
        if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0) {
            perror("Error setting socket option");
            exit(1);
        }
        memset((char *) &serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        serv_addr.sin_port = htons(DISCOVER_PORT);
        if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
            perror("Error binding socket");
            exit(1);
        }
        return sockfd;
    }

    string ListenToClientDiscover(int sockfd)
    {
        cout<<"rodando"<<endl;
        listen(sockfd, 5);
        clilen = sizeof(cli_addr);
        // Accept incoming connections
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
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
        return buff;
    }

    void AddNewClientToTable()
    {
        cout<<"adicionando novo cliente"<<endl;
        for (int i = 0; i < 3; i++)
        {
            
            if(table[i].ip == " ")
            {
                table[i].mac = reinterpret_cast<const char*>(cli_addr.sin_zero);
                table[i].name = inet_ntoa(cli_addr.sin_addr);
                table[i].port = cli_addr.sin_port;
                table[i].ip = inet_ntoa(cli_addr.sin_addr);
                table[i].status = "AWAKEN";
                break;
            }
        }
        close(newsockfd);
    }
    void SendStatusRequest()
    {
        for (int i = 0; i < 3; i++)
        {
            if (table[i].ip != " ")
            {
                char message[] = "sleep status request";
                // Create a new socket for sending messages
                int sendSocket = socket(AF_INET, SOCK_STREAM, 0);
                if (sendSocket < 0) {
                    perror("Error opening send socket");
                    exit(1);
                }

                // Set up the client address
                struct sockaddr_in clientAddr;
                memset((char *) &clientAddr, 0, sizeof(clientAddr));
                clientAddr.sin_family = AF_INET;

                // Set the client IP and port
                clientAddr.sin_addr.s_addr = inet_addr(table[i].ip.c_str());
                clientAddr.sin_port = htons(STATUS_PORT);

                // Connect to the client
                if (connect(sendSocket, (struct sockaddr *) &clientAddr, sizeof(clientAddr)) < 0) {
                    table[i].status = "SLEEPING";
                }
                else
                {
                    table[i].status = "AWAKEN";
                }

                // Close the connection to the client
                close(sendSocket);
            }
        }
    }

    void PrintTable()
    {
        cout << "Name\t\tIP\t\tMAC\t\tStatus\t\tPort" << endl;
        for (int i = 0; i < 3; i++)
        {
            if (table[i].ip != " ")
            {
                cout << table[i].name << "\t" << table[i].ip << "\t" << table[i].mac << "\t" << table[i].status << "\t" << table[i].port << endl;
            }
        }
    }
};

