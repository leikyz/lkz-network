#ifndef PLAYER_MESSAGE_H
#define PLAYER_MESSAGE_H

#include "Message.h"
#include <vector>

struct CreatePlayerMessage  
{
    static int playerId;

    static void Process();

    static std::vector<uint8_t> Serialize();
    static void Deserialize(const std::vector<uint8_t>& data);
};

#endif // PLAYER_MESSAGE_H
