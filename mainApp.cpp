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
            thread threadSendReplication(ThreadSendReplication);

            threadInterface.join();
            threadServerDiscover.join();
            threadServerSleepStatus.join();
            threadSendReplication.join();
            
        }
    } 
    else 
    {
        
        thread threadClient(ThreadStartClient);
        thread threadReplicationListen(ThreadReplicationListen);

        threadClient.join();
        threadReplicationListen.join();

    }

    return 0;
}
