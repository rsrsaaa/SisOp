#include "InterfaceSubservice.h"
#include <unistd.h>

InterfaceSubservice::InterfaceSubservice(ManagementSubservice& managementService)
    : management(managementService) {
}

void InterfaceSubservice::startInterface() {
    thread interfaceThread(&InterfaceSubservice::showParticipants, this);
    interfaceThread.detach();

    while (true) {
        cout << "1. Send Wake Up Command" << endl;
        cout << "2. Exit" << endl;
        cout << "Enter your choice: ";
        int choice;
        cin >> choice;

        switch (choice) {
            case 1:
                sendWakeUpCommand();
                break;
            case 2:
                exit(0);
            default:
                cout << "Invalid choice. Please try again." << endl;
        }
    }
}

void InterfaceSubservice::showParticipants() {
    while (true) {
        cout << "Participants List:" << endl;
        management.displayParticipants();
        sleep(10); // Adjust as needed for refresh interval
    }
}

void InterfaceSubservice::sendWakeUpCommand() {
    // Implementation to send wake up command
    // This can be added based on your existing logic
}
