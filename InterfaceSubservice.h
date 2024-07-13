#ifndef INTERFACESUBSERVICE_H
#define INTERFACESUBSERVICE_H

#include <iostream>
#include <thread>
#include "ManagementSubservice.h"

using namespace std;

class InterfaceSubservice {
public:
    InterfaceSubservice(ManagementSubservice& managementService);
    void startInterface();
    
private:
    void showParticipants();
    void sendWakeUpCommand();

    ManagementSubservice& management;
};

#endif // INTERFACESUBSERVICE_H
