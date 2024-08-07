#include "ServerClass.cpp"
#include "ClientClass.cpp"

pthread_mutex_t lock;

static void ThreadServerDiscover(void *arg) 
{
    Server server;
    string confirmDiscover;     
    int socket = server.InitServerSocket();
    while (1) {
        confirmDiscover = server.ListenToClientDiscover(socket);
        if (confirmDiscover == "sleep discovery service") {
            pthread_mutex_lock(&lock);
            server.AddNewClientToTable();
            pthread_mutex_unlock(&lock);
        }
    }
}

static void ThreadServerSleepStatus(void *arg) 
{
    while (1) {
        Server server;
        pthread_mutex_lock(&lock);
        server.SendStatusRequest();
        pthread_mutex_unlock(&lock);
    }
}

static void ThreadInterface(void *arg) 
{
    Server server;
    int option = 0;
    while (option != 3) 
    {
        cout << "1 - Mostrar tabela de Participantes" << endl << "2 - Wake On LAN" << endl << "3 - Desligar" << endl;
        cin >> option;
        switch (option)
        {
            case 1:
                clearscreen();
                server.PrintTable();
                break;
            case 2:
                //server.WakeOnLAN(string IP);
                break;
            case 3:
                //server.Shutdown();
                break;
            default:
                cout << "Opção inválida" << endl;
                break;
        }
        option = 0;
    }
}

int main(int argc, char *argv[]) {
    pthread_attr_t attr;
    void *res;
    int ret;

    if (argc > 1) {
        if (strcmp(argv[1], "manager") == 0) {
            pthread_t threadServerDiscover, threadServerSleepStatus, threadInterface;
            ret = pthread_mutex_init(&lock, NULL);
            if (ret != 0)
                handle_error_en(ret, "pthread_mutex_init");
            ret = pthread_attr_init(&attr);
            if (ret != 0)
                handle_error_en(ret, "pthread_attr_init");
            ret = pthread_create(&threadServerDiscover, NULL, (void *(*)(void *))ThreadServerDiscover, NULL);
            if (ret != 0)
                handle_error_en(ret, "pthread_create");
            ret = pthread_create(&threadServerSleepStatus, NULL, (void *(*)(void *))ThreadServerSleepStatus, NULL);
            if (ret != 0)
                handle_error_en(ret, "pthread_create");
            ret = pthread_create(&threadInterface, NULL, (void *(*)(void *))ThreadInterface, NULL);
            if (ret != 0)
                handle_error_en(ret, "pthread_create");
            pthread_join(threadServerDiscover, NULL);
            pthread_join(threadServerSleepStatus, NULL);
            pthread_join(threadInterface, NULL);
        }
    } else {
        Client client;
        client.InitClientSocket();
        client.SendRequestToServer();
        client.ListenToServer();
    }

    return 0;
}
