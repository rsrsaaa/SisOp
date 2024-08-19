// mainApp.cpp
#include "threadFunctions.cpp"

int main()
{
    mutex mlock;

    while(true)
    {

        thread threadClient(ThreadStartClient);
        thread threadReplicationListen(ThreadReplicationListen);
        thread clientInterface(ThreadClientInterface);

        threadClient.join();
        threadReplicationListen.join();
        clientInterface.join();
        
        if(!temLider)
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

    return 0;
}
