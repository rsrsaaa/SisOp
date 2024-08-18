// ServerClass.cpp
#include "global.hpp"

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

        std::string received_message(buff);
        size_t delimiter_pos = received_message.find(';');

        std::string mac_address = received_message.substr(delimiter_pos + 1);

        // Armazena o endereço MAC em uma variável temporária
        current_mac = mac_address;

        return received_message.substr(0, delimiter_pos); // Retorna apenas a parte da mensagem sem o MAC
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
                table[i].mac = current_mac; // Armazena o endereço MAC na tabela
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
        cout << "Name\t\tIP\t\tMAC\t\t\t\tStatus\t\tPort" << endl;
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
            repTable.append(table[j].status);
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

                    cout << "rep";
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

    void sendWakeOnLanPacket(const std::string &macAddress)
    {
        unsigned char packet[102];
        int packetIndex = 0;

        // Preenche com 6 bytes de 0xFF
        for (int i = 0; i < 6; ++i)
        {
            packet[packetIndex++] = 0xFF;
        }

        // Converte o endereço MAC para bytes e repete 16 vezes
        unsigned int mac[6];
        sscanf(macAddress.c_str(), "%02x:%02x:%02x:%02x:%02x:%02x", &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);

        for (int i = 0; i < 16; ++i)
        {
            for (int j = 0; j < 6; ++j)
            {
                packet[packetIndex++] = static_cast<unsigned char>(mac[j]);
            }
        }

        // Configuração do socket de transmissão
        int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd < 0)
        {
            perror("Error creating socket");
            return;
        }

        int broadcastEnable = 1;
        if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0)
        {
            perror("Error setting broadcast option");
            close(sockfd);
            return;
        }

        sockaddr_in destAddr;
        memset(&destAddr, 0, sizeof(destAddr));
        destAddr.sin_family = AF_INET;
        destAddr.sin_port = htons(9);                            // Porta padrão para WoL
        destAddr.sin_addr.s_addr = inet_addr("255.255.255.255"); // Envia para todos os dispositivos na rede

        // Envia o pacote WoL
        if (sendto(sockfd, packet, sizeof(packet), 0, (sockaddr *)&destAddr, sizeof(destAddr)) < 0)
        {
            perror("Error sending WakeOnLan packet");
        }

        close(sockfd);
    }
};
