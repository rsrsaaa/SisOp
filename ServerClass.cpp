#include "global.hpp"
#include "netinet/ip_icmp.h"
struct icmp_header
{
    unsigned char type;
    unsigned char code;
    unsigned short checksum;
    unsigned short id;
    unsigned short sequence;
};
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
        int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
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
        clilen = sizeof(cli_addr);
        unsigned int length;
        // Read data from client
        cout<<"esperando dados do cliente"<<endl;
        bzero(buff, sizeof(buff));
        length = sizeof(struct sockaddr_in);
        n = recvfrom(sockfd, buff, 256, 0, (struct sockaddr *) &cli_addr, &length);
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
                struct icmphdr icmp_pkt; 
                struct sockaddr_in dest_addr;
                struct sockaddr_in from;
                
                socklen_t fromlen = sizeof(from);
                dest_addr.sin_addr.s_addr = inet_addr(table[i].ip.c_str());
                int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

                char buffer[1024];

                memset(&icmp_pkt, 0, sizeof(icmp_pkt));
                icmp_pkt.type = ICMP_ECHO;
                icmp_pkt.code = 0; 
                icmp_pkt.un.echo.id = getpid();
                icmp_pkt.un.echo.sequence = 0;
                icmp_pkt.checksum = calculate_checksum(&icmp_pkt, sizeof(icmp_pkt));

                memcpy(buffer, &icmp_pkt, sizeof(icmp_pkt));

                n = sendto(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&dest_addr, sizeof(dest_addr));
                struct timeval timeout;
                timeout.tv_sec = 5;
                timeout.tv_usec = 0;
                setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
                n = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*) &from, &fromlen); 
                if (n < 0)
                {
                    table[i].status = "ASLEEP";
                }
                else
                {
                    cout<<"recebendo mensagem"<<endl;
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
};

