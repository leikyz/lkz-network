#include <string>
#include <memory>


struct Client 
{
    int id;
    sockaddr_in address;
    std::string ipAddress;
    int lobbyId;
    int playerEntityId;
  //  std::shared_ptr<Lobby> lobby;
};
