#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include "global.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include "Management.hpp"

// Função para exibir a interface de usuário/sistema
void showInterface();

extern ClientManager clientManager; // Declare como extern

#endif // INTERFACE_HPP
