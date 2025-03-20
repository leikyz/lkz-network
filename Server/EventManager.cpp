#include "EventManager.h"
#include <iostream>
#include "CreateClientMessage.cpp"
#include "CreateLobbyMessage.cpp"
EventManager::MessageHandler EventManager::messageHandlers[256] = { nullptr };

void EventManager::BindEvents()
{
    EventManager::registerHandler<CreateClientMessage>(1);
    EventManager::registerHandler<CreateLobbyMessage>(2);
}

template<typename T>
void EventManager::registerHandler(int id)
{
    if (id >= 0 && id < 256)
    {
        messageHandlers[id] = &handleMessage<T>;
    }
}

void EventManager::processMessage(std::vector<uint8_t>& buffer, const sockaddr_in& senderAddr)
{
    if (buffer.size() < 4) { // Vérifie qu'on a au moins un int (4 octets)
        return;
    }

    // Lire l'ID en utilisant les 4 premiers octets
    int id = buffer[0] |
        (buffer[1] << 8) |
        (buffer[2] << 16) |
        (buffer[3] << 24);

    // Vérifie si l'ID est valide
    if (id < 0 || id >= 256 || !messageHandlers[id]) {
        return;
    }

    // Supprime les 4 premiers octets du buffer (l'ID)
    buffer.erase(buffer.begin(), buffer.begin() + 4);

    // Appelle le gestionnaire de message avec le buffer modifié (ne contient plus l'ID)
    messageHandlers[id](buffer, senderAddr);
}


template<typename T>
void EventManager::handleMessage(const std::vector<uint8_t>& buffer, const sockaddr_in& senderAddr)
{
    T msg;
    Deserializer deserializer(buffer);
    msg.deserialize(deserializer);
    msg.process(senderAddr);

    const char* yellowColor = "\033[38;5;226m"; // This is a yellow color

    std::cout << yellowColor << "Message received: {"
        << typeid(msg).name() << "}" << std::endl;
}

