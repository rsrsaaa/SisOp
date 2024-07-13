#ifndef MANAGEMENTSUBSERVICE_H
#define MANAGEMENTSUBSERVICE_H

#include <iostream>
#include <vector>

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

class ManagementSubservice {
public:
    void addParticipant(const char* name, const char* ip, int port);
    void updateStatus(const char* ip, bool asleep);
    void displayParticipants();
    
private:
    vector<Participant> participants;
};

#endif // MANAGEMENTSUBSERVICE_H
