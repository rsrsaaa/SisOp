#ifndef DISCOVERYSUBSERVICE_H
#define DISCOVERYSUBSERVICE_H

#include <iostream>
#include <thread>
#include <vector>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>

#define DISCOVERY_PORT_MIN 50000
#define DISCOVERY_PORT_MAX 50099
#define DISCOVERY_MESSAGE "sleep service discovery"
#define BUFFER_SIZE 256

using namespace std;

// Definir a estrutura Participant aqui
#ifndef PARTICIPANT_STRUCT
#define PARTICIPANT_STRUCT
struct Participant {
    char name[20];
    char ip[20];
    int port;
    bool asleep;
};
#endif

class DiscoverySubservice {
public:
    void startDiscovery(bool isManager);
    vector<Participant> getParticipants(); // Função para obter a lista de participantes
    
private:
    void receiveDiscovery();
    void sendDiscovery();

    vector<Participant> participants;
};

#endif // DISCOVERYSUBSERVICE_H
