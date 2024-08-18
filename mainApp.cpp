// mainApp.cpp
#include "threadFunctions.cpp"

int main(int argc, char *argv[])
{
    mutex mlock;

    if (argc > 1)
    {
        if (strcmp(argv[1], "manager") == 0)
        {

            thread threadInterface(ThreadInterface);
            thread threadServerDiscover(ThreadServerDiscover, ref(mlock));
            thread threadServerSleepStatus(ThreadServerSleepStatus, ref(mlock));
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
        thread clientInterface(ThreadClientInterface);

        threadClient.join();
        threadReplicationListen.join();
        clientInterface.join();
    }

    return 0;
}
