//#include "LobbyListMessage.h"
//
//LobbyListMessage::LobbyListMessage() {}
//
//byte LobbyListMessage::getId() const
//{
//    return ID;
//}
//
//std::vector<uint8_t>& LobbyListMessage::serialize(Serializer& serializer) const
//{
//    serializer.writeByte(ID);
//    serializer.writeInt(static_cast<int>(lobbies.size())); 
//
//    for (const auto& lobby : lobbies)
//    {
//        serializer.writeInt(lobby.first); 
//        serializer.writeInt(lobby.second);
//    }
//
//    return serializer.buffer;
//}
//
//void LobbyListMessage::deserialize(Deserializer& deserializer)
//{
//}
//
//void LobbyListMessage::process(const sockaddr_in& senderAddr)
//{
//    lobbies.clear();
//    const auto& allLobbies = LobbyManager::getAllLobbies(); 
//
//    for (const auto& lobby : allLobbies)
//    {
//        int id = lobby->id;
//        int clients = lobby->clients.size();
//        lobbies.emplace_back(id, clients);
//    }
//
//    Serializer serializer;
//    serialize(serializer);
//
//    Server::Send(senderAddr, serializer.buffer);
//}
