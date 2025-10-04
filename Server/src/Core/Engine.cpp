#include "LKZ/Core/Engine.H"
#include <iostream>

void Engine::Run() 
{
    network->Start();

    while (true) {
        network->Poll();
    }
}