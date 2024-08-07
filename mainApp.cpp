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
        sleep(5);
        clearscreen();
        server.PrintTable();
    }
}

int main(int argc, char *argv[]) {
    pthread_attr_t attr;
    void *res;
    int ret;

    if (argc > 1) {
        if (strcmp(argv[1], "manager") == 0) {
            pthread_t threadServerDiscover, threadServerSleepStatus;
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
            pthread_join(threadServerDiscover, NULL);
            pthread_join(threadServerSleepStatus, NULL);
        }
    } else {
        Client client;
        client.InitClientSocket();
        client.SendRequestToServer();
        client.ListenToServer();
    }

    return 0;
}
