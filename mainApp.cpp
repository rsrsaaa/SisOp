#include <iostream>
#include "ServerClass.cpp"
#include "ClientClass.cpp"
using namespace std;

int main() {
    int op;

    cout << "Voce eh um cliente ou um servidor?" << endl;
    cout << "Digite 1 para servidor e 2 para cliente" << endl;
    cin >> op;
    if (op == 1) {
        Server server;
        server.startServer();
    } else {
        Client client;
        client.run();
    }

    return 0;
}