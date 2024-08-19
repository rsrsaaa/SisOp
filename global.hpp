// global.hpp
#pragma once
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/types.h>
#include <mutex>
#include <thread>
#include <net/if.h>
#include <sys/ioctl.h>
#include "netinet/ip_icmp.h"
#include <sstream>
#include <iomanip>
#include <cstring>
#include <cstdlib>
#include <pthread.h>
#include <atomic>
#include <condition_variable>
#include <chrono>
#include <termios.h>

using namespace std;
#define DISCOVER_PORT 53000
#define STATUS_PORT 52000
#define MANAGE_PORT 51000
#define INTERFACE_PORT 50000
#define TABLE_SIZE 4

std::string current_mac;
//std::atomic<bool> user_input(false); // Flag para verificar se o usuário está digitando

int versaoTabela = 0;
struct managementTable
{
    string name = " ";
    string ip = " ";
    string mac = " ";
    string status = " ";
    int port = 0;
    bool isLeader = false;
    int versaoTabela = 0;
};
string currentIP; 
managementTable table[TABLE_SIZE];
int clientNum = 0;
void clearscreen()
{
#ifdef _WIN32
    system("cls");

#elif _POSIX_C_SOURCE >= 199309L
    system("clear");

#endif
}

#define handle_error_en(en, msg) \
    do                           \
    {                            \
        errno = en;              \
        perror(msg);             \
        exit(EXIT_FAILURE);      \
    } while (0)

#define handle_error(msg)   \
    do                      \
    {                       \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while (0)

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
