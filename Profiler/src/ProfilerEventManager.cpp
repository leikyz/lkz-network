#include <iostream>
#include <Profiler/include/Core/Manager/ProfilerEventManager.h>
#include <Common/ProfilerProtocol.h>
#include <Profiler/include/Protocol/Message/Approach/ProfilerClientCreatedMessage.h>
#include <Profiler/include/Protocol/Message/Approach/ProfilerNetworkPerformanceMessage.h>

ProfilerEventManager::MessageHandler ProfilerEventManager::handlers[256] = { nullptr };

void ProfilerEventManager::BindEvents()
{
    std::cout << "[ProfilerEventManager] Binding events..." << std::endl;
    RegisterHandler<ProfilerClientCreatedMessage>(23);
    RegisterHandler<ProfilerNetworkPerformanceMessage>(24);
}

template<typename T>
void ProfilerEventManager::RegisterHandler(uint8_t id)
{
    handlers[id] = &handleMessage<T>;
}
void ProfilerEventManager::ProcessMessage(std::vector<uint8_t>& buffer)
{
    if (buffer.empty()) return;

    uint8_t id = buffer[0];

    if (!handlers[id])
    {
        // std::cerr << "[Profiler] Unknown Packet ID: " << (int)id << std::endl;
        return;
    }

    buffer.erase(buffer.begin());
    // On appelle le lambda créé par RegisterHandler
    handlers[id](buffer);
}

template<typename T>
void ProfilerEventManager::handleMessage(const std::vector<uint8_t>& buffer)
{
    T msg;
    Deserializer deserializer(buffer);

    //std::string name = typeid(msg).name();
    //name = name.substr(7);

    //char ip[INET_ADDRSTRLEN];
    //inet_ntop(AF_INET, &(senderAddr.sin_addr), ip, INET_ADDRSTRLEN);
    //// added +1 because we removed 1 byte before handled event (ID)
    //Logger::Log(std::format("{} ({} bytes) [{}:{}]", name, buffer.size() + 1, ip, ntohs(senderAddr.sin_port)), LogType::Received);

    msg.deserialize(deserializer);
    msg.process();
}