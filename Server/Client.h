struct Client
{
    int id;
    int lobbyId;
    int playerEntityId;
    sockaddr_in address;
    std::string ipAddress;
    uint8_t positionInLobby;
    uint8_t matchmakingMapIdRequest;
    bool isReady;

    Client(int id, const sockaddr_in& addr, const std::string& ip)
        : id(id), lobbyId(-1), playerEntityId(-1),
        address(addr), ipAddress(ip),
        positionInLobby(255), matchmakingMapIdRequest(0), isReady(false)
    {
    }
};
