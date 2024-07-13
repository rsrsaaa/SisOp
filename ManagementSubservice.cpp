#include "ManagementSubservice.h"
#include <string.h>

void ManagementSubservice::addParticipant(const char* name, const char* ip, int port) {
    Participant p;
    strcpy(p.name, name);
    strcpy(p.ip, ip);
    p.port = port;
    p.asleep = false; // Assuming awake initially
    participants.push_back(p);
}

void ManagementSubservice::updateStatus(const char* ip, bool asleep) {
    for (auto& p : participants) {
        if (strcmp(p.ip, ip) == 0) {
            p.asleep = asleep;
            break;
        }
    }
}

void ManagementSubservice::displayParticipants() {
    for (const auto& p : participants) {
        cout << "Participant: " << p.name << " IP: " << p.ip << " Port: " << p.port << " Status: " << (p.asleep ? "Asleep" : "Awaken") << endl;
    }
}
