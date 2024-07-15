#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include <string>
#include <mutex>
#include <vector>
#include <netinet/in.h> // Para sockaddr_in
#include <iostream>     // Para std::ostream

#define DISCOVER_PORT 50000 // Exemplo de porta dentro do range
#define STATUS_PORT 50001   // Exemplo de porta dentro do range
#define MAX_MESSAGE_SIZE 1024
#define DISCOVER_INTERVAL_SECONDS 5

// Declaração externa da variável global
extern bool isRunning;

extern const char *DISCOVERY_MESSAGE;

struct ClientInfo
{
    std::string hostname;
    std::string macAddress;
    std::string ipAddress; // Adicionado membro ipAddress
    int port;               // Adicionado membro port
    std::string status;

    friend std::ostream &operator<<(std::ostream &os, const ClientInfo &client)
    {
        os << "Hostname: " << client.hostname << ", "
           << "MAC Address: " << client.macAddress << ", "
           << "IP Address: " << client.ipAddress << ", "
           << "Port: " << client.port << ", "
           << "Status: " << client.status;
        return os;
    }
};

struct managementTable
{
    // Definição da estrutura managementTable, se necessário
};

class Server; // Declaração antecipada da classe Server para evitar dependências circulares

extern std::vector<ClientInfo> clientTable;
extern std::mutex tableMutex;

void clearscreen();

#define handle_error_en(en, msg)    \
    do                              \
    {                               \
        errno = en;                 \
        perror(msg);                \
        exit(EXIT_FAILURE);         \
    } while (0)

#define handle_error(msg)           \
    do                              \
    {                               \
        perror(msg);                \
        exit(EXIT_FAILURE);         \
    } while (0)

#endif // GLOBAL_HPP
