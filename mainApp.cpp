#include "threadFunctions.cpp"

int main(int argc, char *argv[])
{
    mutex mlock;
    pthread_attr_t attr;
    void *res;
    int ret;

    if (argc > 1) {
        if (strcmp(argv[1], "manager") == 0) 
        {

            thread threadInterface(ThreadInterface,  ref(mlock));
            thread threadServerDiscover(ThreadServerDiscover,  ref(mlock));
            thread threadServerSleepStatus(ThreadServerSleepStatus,  ref(mlock));

            threadInterface.join();
            threadServerDiscover.join();
            threadServerSleepStatus.join();
            
        }
    } 
    else 
    {
        Client client;
        client.InitClientSocket();
        client.SendRequestToServer();
        client.ListenToServer();
    }

    return 0;
}
