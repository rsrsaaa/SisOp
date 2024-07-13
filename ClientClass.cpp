// range de portas 50000 - 54999
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#define PORT 53000
using namespace std;
class Client {
public:
    void run() {
        // range de portas 50000 - 54999
        int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket == -1) {
            cerr << "Failed to create socket." << endl;
            return;
        }

        sockaddr_in serverAddress;
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(PORT);
        serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

        if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
            cerr << "Failed to connect to server." << endl;
            close(clientSocket);
            return;
        }

        char message[] = "sleep discovery service";
        if (send(clientSocket, message, strlen(message), 0) == -1) {
            cerr << "Failed to send message to server." << endl;
            close(clientSocket); 
            return;
        }

        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        if (recv(clientSocket, buffer, sizeof(buffer), 0) == -1) 
        {
            cerr << "Failed to receive message from server." << endl;
            close(clientSocket);
            return;
        }


        if(strcmp(buffer, "discovery ack") == 0) 
        {
            cout << "Server acknowledged discovery request." << endl;
            while(1)
            {
                // Receive sleep status request from server
                // Accept new connection from server
                    // Accept new connection from server
                    socklen_t servlen = sizeof(serverAddress);
                    int newSocket = accept(clientSocket, (struct sockaddr *) &serverAddress, &servlen);
                    if (newSocket == -1) {
                        cerr << "Failed to accept new connection from server." << endl;
                        close(clientSocket);
                        return;
                    }

                char request[1024];
                memset(request, 0, sizeof(request));
                if (recv(clientSocket, request, sizeof(request), 0) == -1) {
                    cerr << "Failed to receive sleep status request from server." << endl;
                    close(clientSocket);
                    return;
                }

                // Check if the request is "sleep status"
                cout<<"sleep status request received"<<endl;
                if (strcmp(request, "sleep status request") == 0) {
                    // Send acknowledgment to server
                    char ack[] = "ack";
                    if (send(clientSocket, ack, strlen(ack), 0) == -1) {
                        cerr << "Failed to send acknowledgment to server." << endl;
                        close(clientSocket);
                        return;
                    }
                
                } else {
                    cerr << "Invalid request received from server." << endl;
                    close(clientSocket);
                    return;
                }
            }
        }
        else 
        {
            cout << "Server did not acknowledge discovery request." << endl;
        }

        close(clientSocket);
    }
};
