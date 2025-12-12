#include "LKZ/Protocol/Message/Approach/CreateClientMessage.h"
#include <Common/ProfilerProtocol.h>

CreateClientMessage::CreateClientMessage() {}

uint8_t CreateClientMessage::getId() const
{
    return ID;
}

std::vector<uint8_t>& CreateClientMessage::serialize(Serializer& serializer) const
{
    serializer.writeByte(ID);
    return serializer.getBuffer();
}

void CreateClientMessage::deserialize(Deserializer& deserializer)
{
   
}

void CreateClientMessage::process(const sockaddr_in& senderAddr)
{
    std::vector<uint8_t> packet;
    packet.push_back(ProfilerProtocol::PacketID::TestMessage);

    // On met une valeur arbitraire (ex: 42) pour vérifier la réception
    int val = 42;
    // Ou le vrai DeltaTime pour voir le graph bouger :
    // int val = static_cast<int>(deltaTime * 1000.0f);

    uint8_t* pVal = reinterpret_cast<uint8_t*>(&val);
    packet.insert(packet.end(), pVal, pVal + sizeof(int));

    // 2. On log coté serveur pour confirmer le départ
    std::cout << "[Main] Manually sending packet to Profiler...\n";

    // 3. Envoi (Thread-Safe grâce au mutex interne du ProfilerServer)
    Engine::Instance().GetProfiler()->Broadcast(packet);

    ClientManager::addClient(senderAddr);

	Client* client = ClientManager::getClientByAddress(senderAddr);

    Serializer serializer;
    serialize(serializer);


        Engine::Instance().Server()->Send(senderAddr, serializer.getBuffer(), getClassName());

}
