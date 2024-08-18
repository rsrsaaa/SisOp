// ClientClass.cpp
#include "global.hpp"

class Client
{
public:
    int clientSocket, clientStatusSocket, clientRepSocket, n;
    sockaddr_in serverAddress;
    int myClientNum = 0;
    string myMAC = getMAC();

    void InitClientSocket()
    {
        clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
        if (clientSocket == -1)
        {
            cerr << "Failed to create client socket." << endl;
            return;
        }

        int broadcastEnable = 1;
        if (setsockopt(clientSocket, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0)
        {
            cerr << "Error setting socket option" << endl;
            return;
        }

        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(DISCOVER_PORT);
        serverAddress.sin_addr.s_addr = inet_addr("255.255.255.255");
    }

    void SendRequestToServer()
    {
        // fazer o cliente mandar o mac por aqui, talvez criando um pacote de mensagem pra enviar o mac junto com a requisição do sleep service
        std::string message = "sleep discovery service;" + myMAC;
        n = sendto(clientSocket, message.c_str(), message.size(), 0, (const struct sockaddr *)&serverAddress, sizeof(struct sockaddr_in));
        if (n < 0)
        {
            cerr << "Failed to send message to server." << endl;
            return;
        }
        myClientNum = clientNum;
        clientNum++;
    }

    void ListenToServer()
    {
        clientStatusSocket = socket(AF_INET, SOCK_DGRAM, 0);
        if (clientStatusSocket == -1)
        {
            cerr << "Failed to create client status socket." << endl;
            return;
        }

        sockaddr_in statusAddress, serverAddress;
        socklen_t servlen;
        statusAddress.sin_family = AF_INET;
        statusAddress.sin_port = htons(STATUS_PORT);
        statusAddress.sin_addr.s_addr = INADDR_ANY;
        servlen = sizeof(struct sockaddr_in);

        if (bind(clientStatusSocket, (struct sockaddr *)&statusAddress, sizeof(statusAddress)) == -1)
        {
            cerr << "Failed to bind status socket." << endl;
            close(clientStatusSocket);
            return;
        }

        char message[1024];
        while (1)
        {
            n = recvfrom(clientStatusSocket, message, 256, 0, (struct sockaddr *)&serverAddress, &servlen);
            if (n < 0)
            {
                cerr << "ERROR on recvfrom" << endl;
                continue;
            }

            n = sendto(clientStatusSocket, "ack", sizeof("ack"), 0, (struct sockaddr *)&serverAddress, sizeof(struct sockaddr));
            if (n < 0)
            {
                cerr << "ERROR on sendto" << endl;
            }
        }
    }

    void ListenToReplication()
    {
        clientRepSocket = socket(AF_INET, SOCK_DGRAM, 0);
        if (clientRepSocket == -1)
        {
            cerr << "Failed to create client replication socket." << endl;
            close(clientRepSocket);
            return;
        }

        sockaddr_in manageAddress, serverAddress;
        socklen_t servlen;
        manageAddress.sin_family = AF_INET;
        manageAddress.sin_port = htons(MANAGE_PORT);
        manageAddress.sin_addr.s_addr = INADDR_ANY;
        servlen = sizeof(struct sockaddr_in);

        if (bind(clientRepSocket, (struct sockaddr *)&manageAddress, sizeof(manageAddress)) == -1)
        {
            cerr << "Failed to bind replication socket." << endl;
            close(clientRepSocket);
            return;
        }

        char message[1024];
        while (1)
        {
            n = recvfrom(clientRepSocket, (void *)message, 1024, 0, (struct sockaddr *)&serverAddress, &servlen);
            if (n < 0)
            {
                cerr << "ERROR on recvfrom" << endl;
                continue;
            }

            string msg = message;
            string delimiter = ";";
            size_t pos = 0;
            string token;
            string res[12] = {""};
            int count = 0;
            while ((pos = msg.find(delimiter)) != string::npos)
            {
                token = msg.substr(0, pos);
                res[count] = token;
                msg.erase(0, pos + delimiter.length());
                count++;
            }

            table[0].ip = res[0];
            table[0].name = res[0];
            table[0].mac = res[1];
            table[0].status = res[2];
            table[0].port = std::stoi(res[3]);
            table[1].ip = res[4];
            table[1].name = res[4];
            table[1].mac = res[5];
            table[1].status = res[6];
            table[1].port = std::stoi(res[7]);
            table[2].ip = res[8];
            table[2].name = res[8];
            table[2].mac = res[9];
            table[2].status = res[10];
            table[2].port = std::stoi(res[11]);
        }
    }

    std::string getMAC()
    {
        struct ifreq ifr;
        struct ifconf ifc;
        char buf[1024];
        int success = 0;

        int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
        if (sock == -1)
        {
            // Handle error
            return "";
        }

        ifc.ifc_len = sizeof(buf);
        ifc.ifc_buf = buf;
        if (ioctl(sock, SIOCGIFCONF, &ifc) == -1)
        {
            // Handle error
            close(sock);
            return "";
        }

        struct ifreq *it = ifc.ifc_req;
        const struct ifreq *const end = it + (ifc.ifc_len / sizeof(struct ifreq));

        for (; it != end; ++it)
        {
            strcpy(ifr.ifr_name, it->ifr_name);
            if (ioctl(sock, SIOCGIFFLAGS, &ifr) == 0)
            {
                if (!(ifr.ifr_flags & IFF_LOOPBACK))
                { // Don't count loopback
                    if (ioctl(sock, SIOCGIFHWADDR, &ifr) == 0)
                    {
                        success = 1;
                        break;
                    }
                }
            }
            else
            {
                // Handle error
                close(sock);
                return "";
            }
        }

        unsigned char mac_address[6];

        if (success)
        {
            memcpy(mac_address, ifr.ifr_hwaddr.sa_data, 6);
        }
        else
        {
            close(sock);
            return "";
        }

        close(sock);

        std::ostringstream mac_stream;
        mac_stream << std::hex << std::setfill('0');
        for (int i = 0; i < 6; ++i)
        {
            mac_stream << std::setw(2) << static_cast<int>(mac_address[i]);
            if (i != 5)
                mac_stream << ":";
        }

        return mac_stream.str();
    }
};
