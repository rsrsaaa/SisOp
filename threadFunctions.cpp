#include "global.hpp"
#include "ServerClass.cpp"
#include "ClientClass.cpp"

//SERVER THREADS BEGIN
void ThreadServerDiscover(mutex& mlock) 
{
    Server server;
    string confirmDiscover;     
    int socket = server.InitServerSocket();
    while (1) 
    {
        confirmDiscover = server.ListenToClientDiscover(socket);
        if (confirmDiscover == "sleep discovery service") 
        {
            lock_guard<mutex> lock(mlock);
            server.AddNewClientToTable();
        }
    }
}

void ThreadServerSleepStatus(mutex& mlock) 
{
    while (1) {
        Server server;
        lock_guard<mutex> lock(mlock);
        server.SendStatusRequest();

    }
}
void ThreadSendReplication() 
{
    Server server;
    while (1) 
    {
        server.SendReplication();
    }
}
//SERVER THREADS END

//INTERFACE THREADS BEGIN
void ThreadInterface(mutex& mlock) 
{
    Server server;
    
    int option = 0;
    while (1) 
    {
        clearscreen();

        server.PrintTable();
        
        sleep(1);
    }
}
//INTERFACE THREADS END

//CLIENT THREADS BEGIN
void ThreadStartClient() 
{
    Client client;
    client.InitClientSocket();
    client.SendRequestToServer();
    client.ListenToServer();
}

void ThreadReplicationListen() 
{
    Client client;
    while (1) 
    {
        client.ListenToReplication();
    }
}
//CLIENT THREADS END
