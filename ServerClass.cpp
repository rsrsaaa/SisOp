#include "ServerClass.h"
#include "global.hpp"
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <algorithm>
#include <string>
#include <chrono>
#include <thread>
#include <netinet/in.h>
#include <sys/socket.h>

// Variáveis globais e definições de mutex
std::vector<ClientInfo> clientTable;
std::mutex tableMutex;
std::unordered_map<std::string, int> clientsTable;
std::mutex clientTableMutex;
extern bool isRunning; // Declaração externa da variável global isRunning

Server::Server()
{
    discoverSocket = 0;
    statusSocket = 0;
}

int Server::InitServerSocket()
{
    // Inicializa o socket UDP para descoberta
    if ((discoverSocket = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        std::cerr << "Failed to create discover server socket." << std::endl;
        return -1;
    }

    struct sockaddr_in discoverAddr;
    std::memset(&discoverAddr, 0, sizeof(discoverAddr));

    discoverAddr.sin_family = AF_INET;
    discoverAddr.sin_addr.s_addr = INADDR_ANY;
    discoverAddr.sin_port = htons(DISCOVER_PORT);

    // Associa o socket à porta e ao endereço para descoberta
    if (bind(discoverSocket, (struct sockaddr *)&discoverAddr, sizeof(discoverAddr)) == -1)
    {
        std::cerr << "Failed to bind discover server socket." << std::endl;
        close(discoverSocket);
        return -1;
    }

    std::cout << "Discover server socket initialized and listening on port " << DISCOVER_PORT << std::endl;

    // Inicializa o socket UDP para status
    if ((statusSocket = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        std::cerr << "Failed to create status server socket." << std::endl;
        return -1;
    }

    struct sockaddr_in statusAddr;
    std::memset(&statusAddr, 0, sizeof(statusAddr));

    statusAddr.sin_family = AF_INET;
    statusAddr.sin_addr.s_addr = INADDR_ANY;
    statusAddr.sin_port = htons(STATUS_PORT);

    // Associa o socket à porta e ao endereço para status
    if (bind(statusSocket, (struct sockaddr *)&statusAddr, sizeof(statusAddr)) == -1)
    {
        std::cerr << "Failed to bind status server socket." << std::endl;
        close(statusSocket);
        return -1;
    }

    std::cout << "Status server socket initialized and listening on port " << STATUS_PORT << std::endl;

    return 0;
}

void Server::ProcessDiscoveryMessage(const std::string &message)
{
    std::string messageCopy = message; // Faz uma cópia da mensagem original

    std::string delimiter = ";";
    size_t pos = 0;
    std::string token;
    std::vector<std::string> tokens;

    while ((pos = messageCopy.find(delimiter)) != std::string::npos)
    {
        token = messageCopy.substr(0, pos);
        tokens.push_back(token);
        messageCopy.erase(0, pos + delimiter.length());
    }
    tokens.push_back(messageCopy); // Último token após o último delimitador

    // Processa os tokens conforme necessário
    std::string clientIP = tokens[0];
    int clientPort = std::stoi(tokens[1]);
    std::string clientHostname = tokens[2];

    // Adiciona informações do cliente à tabela de clientes
    AddClientToTable(clientIP, clientPort, clientHostname);
}

void Server::AddClientToTable(const std::string &clientIP, int clientPort, const std::string &clientHostname)
{
    // Protege o acesso à tabela de clientes com um mutex
    std::lock_guard<std::mutex> guard(clientTableMutex);

    // Verifica se o cliente já está na tabela
    if (clientsTable.find(clientHostname) == clientsTable.end())
    {
        clientsTable[clientHostname] = clientPort;
        std::cout << "Added client " << clientHostname << " with IP " << clientIP << " and port " << clientPort << " to the client table." << std::endl;
    }
    else
    {
        std::cout << "Client " << clientHostname << " with IP " << clientIP << " and port " << clientPort << " is already in the client table." << std::endl;
    }
}

void Server::ListenForDiscoveryMessages()
{
    char buffer[1024] = {0};
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    ssize_t numBytes;

    while (isRunning)
    {
        numBytes = recvfrom(discoverSocket, buffer, sizeof(buffer), 0, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (numBytes == -1)
        {
            std::cerr << "Error receiving discovery message from client." << std::endl;
        }
        else
        {
            std::cout << "Received discovery message from client: " << buffer << std::endl;
            // Processa a mensagem de descoberta recebida
            ProcessDiscoveryMessage(buffer);
        }
    }

    close(discoverSocket); // Fecha o socket de descoberta após a conclusão da execução
}

void Server::SendStatusUpdate(const std::string &clientIP, int clientPort, const std::string &statusMessage)
{
    struct sockaddr_in clientAddr;
    std::memset(&clientAddr, 0, sizeof(clientAddr));

    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons(clientPort);
    clientAddr.sin_addr.s_addr = inet_addr(clientIP.c_str());

    ssize_t numBytes = sendto(statusSocket, statusMessage.c_str(), statusMessage.length(), 0, (struct sockaddr *)&clientAddr, sizeof(clientAddr));
    if (numBytes == -1)
    {
        std::cerr << "Failed to send status update to client " << clientIP << ":" << clientPort << std::endl;
    }
    else
    {
        std::cout << "Sent status update to client " << clientIP << ":" << clientPort << std::endl;
    }
}

void Server::SendShutdownCommand(const std::string &clientHostname)
{
    // Implemente aqui a lógica para enviar um comando de desligamento para um cliente específico
    // Exemplo fictício:
    int clientPort = GetClientPort(clientHostname);
    std::string shutdownCommand = "SHUTDOWN";

    SendStatusUpdate("127.0.0.1", clientPort, shutdownCommand);
}

int Server::GetClientPort(const std::string &clientHostname)
{
    // Protege o acesso à tabela de clientes com um mutex
    std::lock_guard<std::mutex> guard(clientTableMutex);

    // Verifica se o cliente está na tabela de clientes
    if (clientsTable.find(clientHostname) != clientsTable.end())
    {
        return clientsTable[clientHostname];
    }
    else
    {
        std::cerr << "Client " << clientHostname << " not found in the client table." << std::endl;
        return -1;
    }
}

void Server::Start()
{
    // Inicializa o socket do servidor
    if (InitServerSocket() == -1)
    {
        std::cerr << "Failed to initialize server socket. Exiting..." << std::endl;
        return;
    }

    isRunning = true;

    // Cria uma thread separada para ouvir mensagens de descoberta
    std::thread discoveryThread(&Server::ListenForDiscoveryMessages, this);

    // Aguarda a thread de descoberta terminar
    discoveryThread.join();

    // Limpeza: fechar sockets e liberar recursos, se necessário
    close(discoverSocket);
    close(statusSocket);
}

void Server::SendWoLCommand(const std::string& hostname) {
    std::cout << "Sending WoL command to hostname: " << hostname << std::endl;
    // Implemente aqui a lógica para enviar o comando WoL para o cliente
    // Exemplo: use bibliotecas para enviar o pacote WoL para o cliente
}

void Server::PrintClientTable()
{
    // Implementação da função para imprimir a tabela de clientes
    std::cout << "Printing client table:" << std::endl;
    std::cout << "---------------------" << std::endl;
    for (const auto& client : clientTable)
    {
        std::cout << "Hostname: " << client.hostname << ", IP: " << client.ipAddress << ", Port: " << client.port << std::endl;
    }
    std::cout << "---------------------" << std::endl;
}