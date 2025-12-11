#include "LKZ/Core/Engine.h"
#include "LKZ/Utility/Constants.h"
#include <iostream>
#include <thread>
#include <vector>
#include <Common/ProfilerProtocol.h>

INetworkInterface* Engine::network = nullptr;

Engine::Engine(INetworkInterface* netInterface)
{
    network = netInterface;
    // Initialisation du serveur profiler sur le port 5001
    profiler = std::make_unique<ProfilerServer>(ProfilerProtocol::PROFILER_PORT);
}

void Engine::Initialize()
{
    std::cout << "\033[34m";
    std::cout << R"(
                                                       _    _  __ ____           
                                                      | |  | |/ /|_  /           
                                                      | |__| ' <  / /            
                                           _  _ ___ __|____|_|\_\/___|  ___ _  __
                                          | \| | __|_   _\ \    / / _ \| _ \ |/ /
                                          | .` | _|  | |  \ \/\/ / (_) |   / ' < 
                                          |_|\_|___| |_|   \_/\_/ \___/|_|_\_|\_\
                                         
    )" << std::endl;
    std::cout << "\033[0m";
    std::cout << "[Main] Starting engine...\n";

    lastFrame = std::chrono::steady_clock::now();

    profiler->Start();
    ThreadManager::CreatePool("profiler", 1, [this](float dt) {
        if (!profiler) return;

        profiler->Poll();

        static float timer = 0.0f;
        timer += dt;

        if (timer > 0.1f)
        {
            timer = 0.0f;
            float currentDt = this->GetDeltaTime();
            int val = static_cast<int>(currentDt * 1000.0f);

            std::vector<uint8_t> packet;
            packet.push_back(ProfilerProtocol::PacketID::TestMessage);

            uint8_t* pVal = reinterpret_cast<uint8_t*>(&val);
            packet.insert(packet.end(), pVal, pVal + sizeof(int));

            profiler->Broadcast(packet);
        }
        }, true);
}

void Engine::Run()
{
    network->Start();

    const float targetFrameTime = 1.0f / 60.0f;
    float accumulator = 0.0f;

    lastFrame = std::chrono::steady_clock::now();

    while (true)
    {
        auto now = std::chrono::steady_clock::now();
        deltaTime = std::chrono::duration<float>(now - lastFrame).count();
        lastFrame = now;

        accumulator += deltaTime;

        ThreadManager::SetGlobalDeltaTime(deltaTime);

        while (accumulator >= Constants::FIXED_DELTA_TIME)
        {
            ThreadManager::SetGlobalDeltaTime(Constants::FIXED_DELTA_TIME);
            accumulator -= Constants::FIXED_DELTA_TIME;

        }

        network->Poll();

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

INetworkInterface* Engine::Server()
{
    return network;
}