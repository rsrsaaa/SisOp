// threadFunctions.cpp
#include "global.hpp"
#include "ServerClass.cpp"
#include "ClientClass.cpp"

// extern std::atomic<bool> user_input; // Usar atomic para garantir a sincronização
std::mutex mtx;
bool user_input = false;      // Flag para verificar se o usuário está digitando
bool input_processed = false; // Flag para saber se o comando foi processado
std::condition_variable cv;

// SERVER THREADS BEGIN
void ThreadServerDiscover(mutex &mlock)
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

void ThreadServerSleepStatus(mutex &mlock)
{
    while (1)
    {
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
// SERVER THREADS END

// INTERFACE THREADS BEGIN
void ThreadInterface()
{
    Server server;  // Cria uma instância do Server

    std::thread input_thread([&server]() {
        while (true)
        {
            std::string command;
            std::getline(std::cin, command);

            if (!command.empty())
            {
                user_input = true;

                // Remove espaços em branco do início e do final do comando
                command.erase(0, command.find_first_not_of(' '));
                command.erase(command.find_last_not_of(' ') + 1);

                // Verifica se o comando é para Wake-On-LAN
                if (command.find("wakeonlan") == 0)
                {
                    std::string macAddress = command.substr(11);            // Obtém o endereço MAC
                    macAddress.erase(0, macAddress.find_first_not_of(' ')); // Remove espaços em branco
                    server.sendWakeOnLanPacket(macAddress);                 // Chama a função da instância 'server'
                }

                user_input = false;
            }
        }
    });

    input_thread.detach(); // Desconecta a thread de entrada

    while (true)
    {
        if (!user_input)
        { // Só atualiza a tabela se o usuário não estiver digitando
            std::lock_guard<std::mutex> lock(mtx);
            clearscreen();
            server.PrintTable();
        }
         std::this_thread::sleep_for(std::chrono::seconds(1)); // Aguarda 1 segundo antes de atualizar a tabela novamente
    }
}

// INTERFACE THREADS END

// CLIENT THREADS BEGIN
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

void ThreadClientInterface()
{
    Server client;
    // int option = 0;
    while (1)
    {
        clearscreen();
        client.PrintTable();
        sleep(5);
    }
}
// CLIENT THREADS END
