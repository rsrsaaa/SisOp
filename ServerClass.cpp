#include "ServerClass.h"
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include "global.hpp"
#include <algorithm>
#include <string>

Server::Server()
{
    discoverSocket = 0;
    statusSocket = 0;
}

// Exemplo de melhoria na função de inicialização do socket de descoberta
int Server::InitDiscoverSocket()
{
    discoverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (discoverSocket == -1)
    {
        perror("Failed to create discover server socket");
        return -1;
    }

    struct sockaddr_in discoverAddr;
    std::memset(&discoverAddr, 0, sizeof(discoverAddr));
    discoverAddr.sin_family = AF_INET;
    discoverAddr.sin_addr.s_addr = INADDR_ANY;
    discoverAddr.sin_port = htons(DISCOVER_PORT);

    if (bind(discoverSocket, (struct sockaddr *)&discoverAddr, sizeof(discoverAddr)) == -1)
    {
        perror("Failed to bind discover server socket");
        close(discoverSocket);
        return -1;
    }

    std::cout << "Discover server socket initialized and listening on port " << DISCOVER_PORT << std::endl;

    return 0;
}

// Exemplo de função para processar mensagens de descoberta
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

    if (tokens.size() == 4)
    {
        std::string hostname = tokens[0];
        std::string ip = tokens[1];
        std::string mac = tokens[2];
        std::string status = tokens[3];

        UpdateClientTable(hostname, ip, mac, status);
    }
    else
    {
        std::cerr << "Invalid discovery message format." << std::endl;
    }

    PrintTable(); // Exibe a tabela atualizada
}

// Exemplo de função para atualizar a tabela de clientes
void Server::UpdateClientTable(const std::string &hostname, const std::string &ip, const std::string &mac, const std::string &status)
{
    std::lock_guard<std::mutex> guard(tableMutex);

    auto it = std::find_if(clientTable.begin(), clientTable.end(), [&](const ClientInfo &client)
                           { return client.hostname == hostname; });

    if (it != clientTable.end())
    {
        it->status = status;
    }
    else
    {
        ClientInfo newClient;
        newClient.hostname = hostname;
        newClient.ipAddress = ip;
        newClient.macAddress = mac;
        newClient.status = status;
        clientTable.push_back(newClient);
    }
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

void Server::ListenToClientDiscover()
{
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    char buffer[1024] = {0};

    // Aguarda por mensagens de descoberta dos clientes
    ssize_t numBytes = recvfrom(discoverSocket, buffer, sizeof(buffer), 0, (struct sockaddr *)&clientAddr, &clientAddrLen);
    if (numBytes == -1)
    {
        std::cerr << "Error receiving discover data from client." << std::endl;
    }
    else
    {
        std::cout << "Received discovery message from client: " << buffer << std::endl;

        // Processa a mensagem recebida do cliente e atualiza a tabela de clientes, se necessário
        std::string message(buffer);

        // Exemplo de processamento da mensagem
        // Supondo que a mensagem esteja no formato "hostname;ip;mac;status"
        std::string delimiter = ";";
        size_t pos = 0;
        std::string token;
        std::vector<std::string> tokens;

        while ((pos = message.find(delimiter)) != std::string::npos)
        {
            token = message.substr(0, pos);
            tokens.push_back(token);
            message.erase(0, pos + delimiter.length());
        }
        tokens.push_back(message); // Último token após o último delimitador

        if (tokens.size() == 4)
        {
            std::string hostname = tokens[0];
            std::string ip = tokens[1];
            std::string mac = tokens[2];
            std::string status = tokens[3];

            // Mutex para garantir operação segura na tabela de clientes
            std::lock_guard<std::mutex> guard(tableMutex);

            // Verifica se o cliente já existe na tabela
            bool found = false;
            for (auto &client : clientTable)
            {
                if (client.hostname == hostname)
                {
                    // Atualiza o status do cliente
                    client.status = status;
                    found = true;
                    break;
                }
            }

            // Se não encontrou, adiciona o cliente à tabela
            if (!found)
            {
                ClientInfo newClient;
                newClient.hostname = hostname;
                newClient.ipAddress = ip;
                newClient.macAddress = mac;
                newClient.status = status;
                clientTable.push_back(newClient);
            }

            // Atualizações adicionais conforme necessário
            // Por exemplo, atualizar timestamp, contadores, etc.
        }
        else
        {
            std::cerr << "Invalid discovery message format." << std::endl;
        }

        // Exibe a tabela atualizada
        PrintTable();
    }
}

void Server::SendStatusRequest()
{
    // Preparação da mensagem de solicitação de status
    std::string requestMessage = "status_request";

    // Tamanho do buffer para a mensagem
    const int bufferSize = 1024;
    char buffer[bufferSize];

    // Mutex para garantir operação segura na tabela de clientes
    std::lock_guard<std::mutex> guard(tableMutex);

    // Itera sobre a tabela de clientes e envia solicitação de status para cada um
    for (const auto &client : clientTable)
    {
        // Configuração do endereço do cliente
        struct sockaddr_in clientAddr;
        memset(&clientAddr, 0, sizeof(clientAddr));
        clientAddr.sin_family = AF_INET;
        clientAddr.sin_port = htons(DISCOVER_PORT); // Porta de descoberta
        inet_pton(AF_INET, client.ipAddress.c_str(), &clientAddr.sin_addr);

        // Criação do socket UDP para envio
        int discoverSocket = socket(AF_INET, SOCK_DGRAM, 0);
        if (discoverSocket == -1)
        {
            std::cerr << "Failed to create discover socket." << std::endl;
            continue; // Tenta enviar para o próximo cliente
        }

        // Envio da solicitação de status
        ssize_t numBytesSent = sendto(discoverSocket, requestMessage.c_str(), requestMessage.length(), 0,
                                      (struct sockaddr *)&clientAddr, sizeof(clientAddr));
        if (numBytesSent == -1)
        {
            std::cerr << "Failed to send status request to client " << client.hostname << std::endl;
        }
        else
        {
            std::cout << "Sent status request to client " << client.hostname << std::endl;
        }

        // Fecha o socket após o envio
        close(discoverSocket);
    }
}

void Server::PrintTable()
{
    std::lock_guard<std::mutex> guard(tableMutex); // Garante exclusão mútua ao acessar a tabela

    // Verifica se há clientes na tabela
    if (clientTable.empty())
    {
        std::cout << "Client table is empty." << std::endl;
        return;
    }

    // Imprime cabeçalho da tabela
    std::cout << "================ Client Table ================" << std::endl;
    std::cout << "Hostname\tIP Address\tMAC Address\tStatus" << std::endl;

    // Itera sobre a tabela de clientes e imprime cada cliente
    for (const auto &client : clientTable)
    {
        std::cout << client.hostname << "\t"
                  << client.ipAddress << "\t"
                  << client.macAddress << "\t"
                  << client.status << std::endl;
    }

    // Imprime rodapé da tabela
    std::cout << "==============================================" << std::endl;
}

void Server::UpdateClientStatus(const std::string &hostname, const std::string &status)
{
    std::lock_guard<std::mutex> guard(tableMutex); // Garante exclusão mútua ao acessar a tabela

    // Procura o cliente na tabela pelo hostname
    auto it = std::find_if(clientTable.begin(), clientTable.end(), [&](const ClientInfo &client)
                           { return client.hostname == hostname; });

    if (it != clientTable.end())
    {
        // Atualiza o status do cliente encontrado
        it->status = status;
        std::cout << "Updated status of client " << hostname << " to " << status << std::endl;
    }
    else
    {
        // Cliente não encontrado na tabela
        std::cerr << "Client " << hostname << " not found in client table." << std::endl;
    }
}

void Server::AddNewClientToTable(const ClientInfo &client)
{
    std::lock_guard<std::mutex> guard(tableMutex); // Garante exclusão mútua ao acessar a tabela

    // Verifica se o cliente já existe na tabela pelo hostname
    auto it = std::find_if(clientTable.begin(), clientTable.end(), [&](const ClientInfo &existingClient)
                           { return existingClient.hostname == client.hostname; });

    if (it == clientTable.end())
    {
        // Cliente não encontrado na tabela, pode adicionar
        clientTable.push_back(client);
        std::cout << "Added new client to the table: " << client.hostname << std::endl;
    }
    else
    {
        // Cliente já existe na tabela, atualiza seus dados se necessário
        *it = client;
        std::cout << "Updated existing client in the table: " << client.hostname << std::endl;
    }
}
