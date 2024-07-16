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

using namespace std;
#define DISCOVER_PORT 53000
#define STATUS_PORT 52000

unsigned short calculate_checksum(void *b, int len)
{
    unsigned short *buf = static_cast<unsigned short *>(b);
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char *)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

struct managementTable
{
    string name = " ";
    string ip = " ";
    string mac = " ";
    string status = " ";
    int port = 0;
};
managementTable table[3];
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