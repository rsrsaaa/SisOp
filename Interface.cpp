#include "Interface.hpp"
#include "Management.hpp"
#include <iostream>
#include <chrono>

extern ClientManager clientManager; // Referência externa para o gerenciador de clientes definido em Management.cpp

// Função para limpar a tela, dependendo do sistema operacional
void clearScreen() {
#ifdef _WIN32
    system("cls");
#elif _POSIX_C_SOURCE >= 199309L
    system("clear");
#endif
}

// Implementação da função para exibir a interface de usuário/sistema
void showInterface() {
    while (true) {
        clearScreen();
        cout << "System Management Interface" << endl;
        cout << "===========================" << endl;
        cout << "Name\t\tIP\t\tMAC\t\tStatus\t\tPort" << endl;
        
        vector<managementTable> clients = clientManager.getClientTable();
        for (auto& client : clients) {
            if (!client.ip.empty()) {
                cout << client.name << "\t" << client.ip << "\t" << client.mac << "\t\t" << client.status << "\t\t" << client.port << endl;
            }
        }
        
        // Opções de interação
        cout << endl << "Options:" << endl;
        cout << "1. Refresh Status" << endl;
        cout << "2. Add Client" << endl; // Implementar se necessário
        cout << "3. Remove Client" << endl; // Implementar se necessário
        cout << "4. Exit" << endl;
        
        int choice;
        cout << "Enter your choice: ";
        cin >> choice;
        
        switch (choice) {
            case 1:
                // Atualiza status dos clientes
                clientManager.refreshClientStatus();
                break;
            case 2:
                // Adicionar cliente (opcional)
                // Implementar se necessário
                break;
            case 3:
                // Remover cliente (opcional)
                // Implementar se necessário
                break;
            case 4:
                // Sair do programa
                cout << "Exiting..." << endl;
                return;
            default:
                cout << "Invalid choice. Please try again." << endl;
                break;
        }
        
        // Aguarda um pouco antes de atualizar a tela novamente (5 segundos neste exemplo)
        this_thread::sleep_for(chrono::seconds(5));
    }
}
