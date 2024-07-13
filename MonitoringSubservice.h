#ifndef MONITORINGSUBSERVICE_H
#define MONITORINGSUBSERVICE_H

#include <iostream>
#include <thread>
#include <vector>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>

#define STATUS_PORT_MIN 50100
#define STATUS_PORT_MAX 50199
#define STATUS_REQUEST_MESSAGE "sleep status request"
#define BUFFER_SIZE 256

using namespace std;

class MonitoringSubservice {
public:
    void startMonitoring(bool isManager);
    
private:
    void receiveStatusRequests();
    void sendStatusRequests();

    vector<string> asleepParticipants;
};

#endif // MONITORINGSUBSERVICE_H
