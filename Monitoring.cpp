#include "Monitoring.hpp"
#include <chrono>
#include <thread>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

using namespace std;

// Função para realizar o monitoramento do status dos clientes via UDP
void startMonitoring() {
    while (true) {
        vector<managementTable> currentClients = clientManager.getClientTable();
        
        for (auto& client : currentClients) {
            if (client.ip != " ") {
                struct sockaddr_in serverAddress;
                int clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
                if (clientSocket == -1) {
                    cerr << "Failed to create client socket for monitoring." << endl;
                    continue;
                }
                
                serverAddress.sin_family = AF_INET;
                serverAddress.sin_port = htons(STATUS_PORT);
                serverAddress.sin_addr.s_addr = inet_addr(client.ip.c_str());
                
                char message[] = "status request";
                if (sendto(clientSocket, message, strlen(message), 0, (const struct sockaddr *) &serverAddress, sizeof(struct sockaddr_in)) < 0) {
                    cerr << "Failed to send status request to client " << client.ip << endl;
                }
                
                char buffer[MAX_MESSAGE_SIZE];
                socklen_t serverlen = sizeof(serverAddress);
                int n = recvfrom(clientSocket, buffer, MAX_MESSAGE_SIZE, 0, (struct sockaddr *) &serverAddress, &serverlen);
                if (n < 0) {
                    cerr << "Failed to receive status response from client " << client.ip << endl;
                } else {
                    buffer[n] = '\0';
                    string status = string(buffer);
                    clientManager.updateClientStatus(client.ip, status);
                }
                
                close(clientSocket);
            }
        }
        
        // Atualiza a tela com a tabela de clientes
        clearScreen();
        cout << "Monitoring Client Status..." << endl;
        cout << "Name\t\tIP\t\tMAC\t\tStatus\t\tPort" << endl;
        vector<managementTable> updatedClients = clientManager.getClientTable();
        for (auto& client : updatedClients) {
            if (client.ip != " ") {
                cout << client.name << "\t" << client.ip << "\t" << client.mac << "\t\t" << client.status << "\t\t" << client.port << endl;
            }
        }
        
        // Intervalo de monitoramento (5 segundos neste exemplo)
        this_thread::sleep_for(chrono::seconds(5));
    }
}
