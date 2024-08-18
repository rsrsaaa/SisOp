//global.hpp
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


using namespace std;
#define DISCOVER_PORT 53000
#define STATUS_PORT 52000
#define MANAGE_PORT 51000
#define INTERFACE_PORT 50000

int versaoTabela = 0;
struct managementTable
{
    string name = " ";
    string ip = " ";
    string mac = " ";
    string status = " ";
    int port = 0;
    int versaoTabela = 0;
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