//#include "SynchronizeEntitiesMessage.h"
//#include "CreateEntityMessage.h"
//
//SynchronizeEntitiesMessage::SynchronizeEntitiesMessage() {}
//
//byte SynchronizeEntitiesMessage::getId() const
//{
//    return ID;
//}
//
//std::vector<uint8_t>& SynchronizeEntitiesMessage::serialize(Serializer& serializer) const
//{
//    serializer.writeByte(ID);
//    return serializer.buffer;
//}
//
//void SynchronizeEntitiesMessage::deserialize(Deserializer& deserializer)
//{
//
//}
//
//void SynchronizeEntitiesMessage::process(const sockaddr_in& senderAddr)
//{
//    std::shared_ptr<Lobby> lobby = LobbyManager::getLobby(ClientManager::getClientByAddress(senderAddr)->lobbyId);
//
//    if (!lobby)
//    {
//        std::cerr << "Lobby introuvable pour le client." << std::endl;
//        return;
//    }
//
//    for (const auto& entity : lobby->entities)
//    {
//        if (entity->id != ClientManager::getClientByAddress(senderAddr)->playerEntityId)
//        {
//            CreateEntityMessage createEntityMessage(entity->id, entity->type, entity->posX, entity->posY, entity->posZ);
//
//            Serializer serializer;
//            createEntityMessage.serialize(serializer);
//
//            Server::Send(senderAddr, serializer.buffer);
//        }
//    }
//}
