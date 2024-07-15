#include "ClientClass.h"
#include "global.hpp"
#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

Client::Client()
{
    discoverSocket = 0;
    statusSocket = 0;
    serverIP = "127.0.0.1"; // Exemplo: inicializa com um endereço IP padrão
    serverPort = 50000;     // Exemplo: inicializa com uma porta padrão
    asleep = false;         // Exemplo: inicializa como falsa (estação desperta)
}

void Client::InitClientSocket()
{
    // Inicializa o socket UDP para descoberta
    if ((discoverSocket = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        std::cerr << "Failed to create discover client socket." << std::endl;
        exit(EXIT_FAILURE);
    }

    // Configura o endereço do servidor para descoberta
    std::memset(&serverAddrDiscover, 0, sizeof(serverAddrDiscover));
    serverAddrDiscover.sin_family = AF_INET;
    serverAddrDiscover.sin_port = htons(DISCOVER_PORT);               // Porta de descoberta
    serverAddrDiscover.sin_addr.s_addr = inet_addr(serverIP.c_str()); // Converte o endereço IP para o formato apropriado

    // Inicializa o socket UDP para status
    if ((statusSocket = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        std::cerr << "Failed to create status client socket." << std::endl;
        exit(EXIT_FAILURE);
    }

    // Configura o endereço do servidor para status
    std::memset(&serverAddrStatus, 0, sizeof(serverAddrStatus));
    serverAddrStatus.sin_family = AF_INET;
    serverAddrStatus.sin_port = htons(STATUS_PORT);                 // Porta de status
    serverAddrStatus.sin_addr.s_addr = inet_addr(serverIP.c_str()); // Converte o endereço IP para o formato apropriado
}

void Client::SendDiscoveryMessage()
{
    std::string discoveryMessage = "sleep service discovery";
    std::lock_guard<std::mutex> guard(socketMutex); // Garante exclusão mútua

    ssize_t sentBytes = sendto(discoverSocket, discoveryMessage.c_str(), discoveryMessage.length(), 0,
                               (struct sockaddr *)&serverAddrDiscover, sizeof(serverAddrDiscover));
    if (sentBytes == -1)
    {
        std::cerr << "Failed to send discovery message." << std::endl;
    }
    else
    {
        std::cout << "Discovery message sent to server." << std::endl;
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
