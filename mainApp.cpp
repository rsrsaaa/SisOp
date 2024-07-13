#include <iostream>
#include <thread>
#include "ServerClass.h"
#include "ClientClass.h"
#include "ManagementSubservice.h"
#include "InterfaceSubservice.h"
#include "DiscoverySubservice.h"
#include "MonitoringSubservice.h"

using namespace std;

int main() {
    int choice;

    cout << "Digite 1 para servidor e 2 para cliente" << endl;
    cin >> choice;

    if (choice == 1) {
        ManagementSubservice managementService;
        DiscoverySubservice discoveryService;
        MonitoringSubservice monitoringService;

        discoveryService.startDiscovery(true);
        monitoringService.startMonitoring(true);

        InterfaceSubservice interfaceService(managementService);
        interfaceService.startInterface();
    } else {
        Client client;
        client.run();

        // Exibir os participantes após a execução do cliente
        DiscoverySubservice discoveryService;
        vector<Participant> participants = discoveryService.getParticipants();
        
        cout << "Participants List:" << endl;
        for (const auto& participant : participants) {
            cout << "Name: " << participant.name << " IP: " << participant.ip
                 << " Port: " << participant.port << " Status: " << (participant.asleep ? "Asleep" : "Awaken") << endl;
        }
    }

    return 0;
}
