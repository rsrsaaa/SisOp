#include "global.hpp"
class Client {
public:
    int clientSocket, clientStatusSocket, n;
    sockaddr_in serverAddress;
    int myClientNum = 0;
    void InitClientSocket() 
    {
        
        cout<<"rodando cliente"<<endl;
        clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
        if (clientSocket == -1) {
            cerr << "Failed to create client socket." << endl;
            return;
        }
        
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(DISCOVER_PORT);
        serverAddress.sin_addr.s_addr = inet_addr("255.255.255.255");
        
    }


    void SendRequestToServer() 
    {
        cout<<"enviando mensagem para o servidor"<<endl;
        char message[] = "sleep discovery service";
        n = sendto(clientSocket, message, strlen(message), 0, (const struct sockaddr *) &serverAddress, sizeof(struct sockaddr_in));
        myClientNum = clientNum;
        clientNum++;
        close(clientSocket);
    }

    void ListenToServer()
    {
        clientStatusSocket = socket(AF_INET, SOCK_DGRAM, 0);
        if (clientStatusSocket == -1) {
            cerr << "Failed to create client socket." << endl;
            return;
        }
        cout << "Listening for server connections on the status port." << endl; // Replace with the actual status port number
        sockaddr_in statusAddress, serverAddress;
        socklen_t servlen;
        statusAddress.sin_family = AF_INET;
        statusAddress.sin_port = htons(STATUS_PORT);
        statusAddress.sin_addr.s_addr = INADDR_ANY;
        servlen = sizeof(struct sockaddr_in);
        if (bind(clientStatusSocket, (struct sockaddr*)&statusAddress, sizeof(statusAddress)) == -1) {
            cerr << "Failed to bind status socket." << endl;
            close(clientStatusSocket);
            return;
        }

        char message[1024];
        unsigned int length;
        length = sizeof(struct sockaddr_in);
        while(1)
        {
            n = recvfrom(clientStatusSocket, message, 256, 0, (struct sockaddr *) &serverAddress, &servlen);
            if (n < 0) 
                printf("ERROR on recvfrom");
            printf("Received a datagram: %s\n", message);
            
            /* send to socket */
            n = sendto(clientStatusSocket, "ack", sizeof("ack"), 0,(struct sockaddr *) &serverAddress, sizeof(struct sockaddr));
            if (n  < 0) 
			printf("ERROR on sendto");
        }


    }
    // void run() {
    //         cout << "Server acknowledged discovery request." << endl;
    //         while(1)
    //         {
    //             // Receive sleep status request from server
    //             // Accept new connection from server
    //                 // Accept new connection from server
    //                 socklen_t servlen = sizeof(serverAddress);
    //                 int newSocket = accept(clientSocket, (struct sockaddr *) &serverAddress, &servlen);
    //                 if (newSocket == -1) {
    //                     cerr << "Failed to accept new connection from server." << endl;
    //                     close(clientSocket);
    //                     return;
    //                 }

    //             char request[1024];
    //             memset(request, 0, sizeof(request));
    //             if (recv(clientSocket, request, sizeof(request), 0) == -1) {
    //                 cerr << "Failed to receive sleep status request from server." << endl;
    //                 close(clientSocket);
    //                 return;
    //             }

    //             // Check if the request is "sleep status"
    //             cout<<"sleep status request received"<<endl;
    //             if (strcmp(request, "sleep status request") == 0) {
    //                 // Send acknowledgment to server
    //                 char ack[] = "ack";
    //                 if (send(clientSocket, ack, strlen(ack), 0) == -1) {
    //                     cerr << "Failed to send acknowledgment to server." << endl;
    //                     close(clientSocket);
    //                     return;
    //                 }
                
    //             } else {
    //                 cerr << "Invalid request received from server." << endl;
    //                 close(clientSocket);
    //                 return;
    //             }
    //         }
    //     }
    //     else 
    //     {
    //         cout << "Server did not acknowledge discovery request." << endl;
    //     }

    //     close(clientSocket);
    // }
};
