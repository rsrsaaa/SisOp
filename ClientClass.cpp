#include "ClientClass.h"
#include "global.hpp"
#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include <chrono>

Client::Client()
{
    memset(&discoverClientAddr, 0, sizeof(discoverClientAddr));
    discoverClientAddr.sin_family = AF_INET;
    discoverClientAddr.sin_port = htons(DISCOVER_PORT);
    discoverClientAddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
}

void Client::InitDiscoverSocket()
{
    discoverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (discoverSocket < 0)
    {
        std::cerr << "Failed to create discover socket." << std::endl;
        exit(EXIT_FAILURE);
    }

    int broadcastEnable = 1;
    if (setsockopt(discoverSocket, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0)
    {
        std::cerr << "Failed to enable broadcast on discover socket." << std::endl;
        close(discoverSocket); // Fechar o socket em caso de erro
        exit(EXIT_FAILURE);
    }

    // Bind o socket a uma porta específica para garantir o envio correto
    struct sockaddr_in clientAddr;
    memset(&clientAddr, 0, sizeof(clientAddr));
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons(0); // Bind a qualquer porta disponível
    clientAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(discoverSocket, (struct sockaddr *)&clientAddr, sizeof(clientAddr)) < 0)
    {
        std::cerr << "Failed to bind discover socket." << std::endl;
        close(discoverSocket); // Fechar o socket em caso de erro
        exit(EXIT_FAILURE);
    }
}

void Client::ListenForStatusUpdates()
{
    char buffer[1024] = {0};
    struct sockaddr_in serverAddr;
    socklen_t serverAddrLen = sizeof(serverAddr);
    ssize_t numBytes;

    bool shouldExit = false; // Flag para indicar quando sair do loop

    while (!shouldExit)
    {
        {
            std::lock_guard<std::mutex> guard(socketMutex); // Garante exclusão mútua

            // Recebe atualizações de status do servidor
            numBytes = recvfrom(statusSocket, buffer, sizeof(buffer), 0, (struct sockaddr *)&serverAddr, &serverAddrLen);
        }

        if (numBytes == -1)
        {
            std::cerr << "Error receiving status update from server." << std::endl;
        }
        else
        {
            std::cout << "Received status update from server: " << buffer << std::endl;

            // Processa a mensagem de atualização de status recebida
            std::string message(buffer);
            // Verifica se a mensagem indica que o cliente deve sair
            if (message == "EXIT")
            {
                shouldExit = true;
                std::cout << "Exiting status update listener..." << std::endl;
                // Aqui você pode adicionar código para realizar outras operações necessárias antes de sair do loop
                // Por exemplo, enviar uma confirmação ao servidor de que o cliente está saindo
            }
        }
    }

    // Após sair do loop, aqui você pode realizar outras operações de limpeza, como fechar sockets
    close(statusSocket); // Fecha o socket de status
    // Outras operações de limpeza, se necessário
}

void Client::SendStatusUpdate()
{
    // Exemplo simples: enviar um pacote UDP com o status
    std::string statusMessage = asleep ? "ASLEEP" : "AWAKEN";

    // Envio da mensagem UDP com o status para o servidor
    SendUDPMessage(serverIP, serverPort, statusMessage);
}

void Client::ProcessCommand(const std::string &command)
{
    if (command.substr(0, 7) == "WAKEUP ")
    {
        std::string hostname = command.substr(7);
        // Implemente a lógica para enviar o comando WAKEUP para o hostname especificado
        WakeUpStation(hostname);
    }
    else
    {
        // Implemente outros comandos conforme necessário
    }
}

void Client::WakeUpStation(const std::string &hostname)
{
    // Implemente aqui a lógica para enviar o comando WoL para acordar uma estação específica
    // Exemplo fictício:
    SendWoLCommand(hostname);
}

void Client::SendWoLCommand(const std::string &hostname)
{
    // Implemente aqui a lógica real para enviar o comando Wake-On-LAN (WoL)
    // Exemplo fictício:
    std::string wolCommand = "WOL " + hostname;
    SendUDPMessage(serverIP, serverPort, wolCommand);
}

void Client::SendUDPMessage(const std::string &ip, int port, const std::string &message)
{
    struct sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));

    // Configuração do endereço do servidor
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());

    // Código para enviar mensagem UDP
}

void Client::SendDiscoveryMessages()
{
    int broadcastPermission = 1;
    if (setsockopt(discoverSocket, SOL_SOCKET, SO_BROADCAST, (void *)&broadcastPermission, sizeof(broadcastPermission)) < 0)
        perror("setsockopt() failed");

    while (true)
    {
        sendto(discoverSocket, DISCOVERY_MESSAGE, strlen(DISCOVERY_MESSAGE), 0, (struct sockaddr *)&discoverClientAddr, sizeof(discoverClientAddr));
        std::this_thread::sleep_for(std::chrono::seconds(DISCOVER_INTERVAL_SECONDS));
    }
}
