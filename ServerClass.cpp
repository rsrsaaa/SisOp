#include "global.hpp"
#include "netinet/ip_icmp.h"
#include <sys/ioctl.h> // Add this line to include the necessary header file
#include <sys/ioctl.h> // Add this line to include the necessary header file

class Server
{
public:
    int newsockfd, n;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    char buff[256];

    int InitServerSocket()
    {
        int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd < 0)
        {
            perror("Error opening socket");
            exit(1);
        }
        int broadcastEnable = 1;
        if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0)
        {
            perror("Error setting socket option");
            exit(1);
        }
        memset((char *)&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        serv_addr.sin_port = htons(DISCOVER_PORT);
        if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        {
            perror("Error binding socket");
            exit(1);
        }
        return sockfd;
    }

    string ListenToClientDiscover(int sockfd)
    {
        clilen = sizeof(cli_addr);
        unsigned int length;
        bzero(buff, sizeof(buff));
        length = sizeof(struct sockaddr_in);
        n = recvfrom(sockfd, buff, 256, 0, (struct sockaddr *)&cli_addr, &length);
        if (n < 0)
        {
            perror("Error reading from socket");
            exit(1);
        }
        return buff;
    }

    void AddNewClientToTable()
    {

        for (int i = 0; i < 3; i++)
        {
            if (table[i].ip == " ")
            {

                table[i].name = inet_ntoa(cli_addr.sin_addr);
                table[i].port = cli_addr.sin_port;
                table[i].ip = inet_ntoa(cli_addr.sin_addr);
                table[i].status = "AWAKEN";
                sleep(1);
                versaoTabela++;
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
                int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
                if (sockfd < 0)
                {
                    perror("Error opening socket");
                    continue;
                }

                struct sockaddr_in dest_addr;
                memset(&dest_addr, 0, sizeof(dest_addr));
                dest_addr.sin_family = AF_INET;
                dest_addr.sin_port = htons(STATUS_PORT);
                dest_addr.sin_addr.s_addr = inet_addr(table[i].ip.c_str());

                const char *status_request = "sleep status request";
                n = sendto(sockfd, status_request, strlen(status_request), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
                if (n < 0)
                {
                    perror("Error sending status request");
                    close(sockfd);
                    continue;
                }

                char buffer[256];
                struct timeval timeout;
                timeout.tv_sec = 5;
                timeout.tv_usec = 0;
                setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout));

                struct sockaddr_in from;
                socklen_t fromlen = sizeof(from);
                n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&from, &fromlen);
                if (n < 0)
                {
                    table[i].status = "ASLEEP";
                }
                else
                {
                    table[i].status = "AWAKEN";
                }

                close(sockfd);
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
                cout << table[i].name << "\t" << table[i].ip << "\t" << table[i].mac << "\t\t" << table[i].status << "\t\t" << table[i].port << endl;
            }
        }
    }

    void SendReplication()
    {
            string repTable;
            for (int j = 0; j < 3; j++)
            {
                repTable.append(table[j].ip);
                repTable.append(";");
                repTable.append(table[j].mac);
                repTable.append(";");
                repTable.append(std::to_string(table[j].port));
                repTable.append(";");
            }
            
            for (int i = 0; i < 3; i++)
            {
                if (table[i].ip != " ")
                {
                    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
                    if (sockfd < 0)
                    {
                        perror("Error opening socket");
                        continue;
                    }

                    struct sockaddr_in dest_addr;
                    memset(&dest_addr, 0, sizeof(dest_addr));
                    dest_addr.sin_family = AF_INET;
                    dest_addr.sin_port = htons(MANAGE_PORT);
                    dest_addr.sin_addr.s_addr = inet_addr(table[i].ip.c_str());

                    // montar mensagem de replicação

                    
                    if (versaoTabela > table[i].versaoTabela && table[i].ip != " ")
                    {
                        table[i].versaoTabela = versaoTabela;
                        
                        cout<<"rep";
                        n = sendto(sockfd, repTable.c_str(), strlen(repTable.c_str()), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
                        if (n < 0)
                        {
                            perror("Error sending replication");
                            close(sockfd);
                            continue;
                        }
                    }
                    close(sockfd);
                }
            }
            

        
    }

    void sendWOL(const std::string &macAddress, const std::string &broadcastAddress, int port = 9)
    {
        // Create the magic packet
        unsigned char packet[102];

        // 6 bytes of 0xFF
        std::fill_n(packet, 6, 0xFF);

        // 16 repetitions of the MAC address
        unsigned char mac[6];
        sscanf(macAddress.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
               &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);

        for (int i = 1; i <= 16; ++i)
        {
            std::copy(mac, mac + 6, packet + i * 6);
        }

        // Create a UDP socket
        int sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock < 0)
        {
            perror("socket");
            return;
        }

        // Set socket options to allow broadcast
        int optval = 1;
        if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval)) < 0)
        {
            perror("setsockopt");
            close(sock);
            return;
        }

        // Configure the destination address
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = inet_addr(broadcastAddress.c_str());

        // Send the magic packet
        if (sendto(sock, packet, sizeof(packet), 0, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        {
            perror("sendto");
        }

        // Close the socket
        close(sock);
    }
};
