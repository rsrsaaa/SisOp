// threadFunctions.cpp
#include "global.hpp"
#include "ServerClass.cpp"
#include "ClientClass.cpp"

// extern std::atomic<bool> user_input; // Usar atomic para garantir a sincronização
std::mutex mtx;
std::atomic<bool> user_input(false); // Flag para verificar se o usuário está digitando
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
// Função para atualizar a interface e tabela
void ThreadInterface()
{
    Server server; // Cria uma instância do Server

    // Thread para capturar entrada do usuário
    std::thread input_thread([&server]()
                             {
        std::string command;
        while (true)
        {
            std::getline(std::cin, command); // Lê a entrada do usuário

            if (!command.empty())
            {
                user_input = true; // Indica que o usuário está digitando

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

                user_input = false; // Comando processado, usuário não está mais digitando
            }
        } });

    input_thread.detach(); // Desconecta a thread de entrada

    while (true)
    {
        std::unique_lock<std::mutex> lock(mtx);

        if (!user_input) // Só atualiza a tabela se o usuário não estiver digitando
        {
            clearscreen();
            server.PrintTable();
            std::cout << "\nserver$: "; // Prompt de comando visível para o usuário
            std::cout.flush();          // Garante que o prompt seja exibido imediatamente
        }
        else
        {
            // Se o usuário está digitando, só atualize o prompt
            std::cout << "\rserver$: " << std::flush; // Mantém o prompt na mesma linha
        }

        lock.unlock();                                         // Libera o mutex para outras operações
        std::this_thread::sleep_for(std::chrono::seconds(10)); // Aguarda 1 segundo antes de atualizar a tabela novamente
    }
}
// INTERFACE THREADS END

// CLIENT THREADS BEGIN
void ThreadStartClient()
{
    Client client;
    client.InitClientSocket();
    client.SendRequestToServer();
    if (temLider)
        client.ListenToServer();
}

void ThreadReplicationListen()
{
    Client client;
    sleep(1);
    while (temLider)
    {

        client.ListenToReplication();
    }
}

void ThreadClientInterface()
{
    Server client;
    sleep(1);
    while (temLider)
    {
        clearscreen();
        client.PrintTable();
        sleep(5);
    }
}
// CLIENT THREADS END
