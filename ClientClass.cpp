#include "global.hpp"
class Client {
public:
    int clientSocket, clientStatusSocket;
    sockaddr_in serverAddress;

    void InitClientSocket() 
    {
        
        cout<<"rodando cliente"<<endl;
        clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket == -1) {
            cerr << "Failed to create client socket." << endl;
            return;
        }
        
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(DISCOVER_PORT);
        serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
        
    }

    void ConnectToServer() 
    {
        
        cout<<"conectando ao servidor"<<endl;
        if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
            std::cerr << "Failed to connect to server." << std::endl;
            close(clientSocket);
            return;
        }
    }

    void SendRequestToServer() 
    {
        cout<<"enviando mensagem para o servidor"<<endl;
        char message[] = "sleep discovery service";
        if (send(clientSocket, message, strlen(message), 0) == -1) {
            cerr << "Failed to send message to server." << endl;
            close(clientSocket);
            return;
        }
    }
    void ListenToServer() 
    {
        cout << "Listening for server connections on the status port." << endl; // Replace with the actual status port number
        sockaddr_in statusAddress;
        statusAddress.sin_family = AF_INET;
        statusAddress.sin_port = htons(STATUS_PORT);
        statusAddress.sin_addr.s_addr = INADDR_ANY;

        if (bind(clientStatusSocket, (struct sockaddr*)&statusAddress, sizeof(statusAddress)) == -1) {
            cerr << "Failed to bind status socket." << endl;
            close(clientStatusSocket);
            return;
        }

        if (listen(clientStatusSocket, 5) == -1) {
            cerr << "Failed to listen for server connections." << endl;
            close(clientStatusSocket);
            return;
        }

        while (1) {
            socklen_t servlen = sizeof(serverAddress);
            int newSocket = accept(clientStatusSocket, (struct sockaddr*)&serverAddress, &servlen);
            if (newSocket == -1) {
                cerr << "Failed to accept new connection from server." << endl;
                close(clientStatusSocket);
                return;
            }

            // Handle the new connection from the server
            // ...

            close(newSocket);
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
