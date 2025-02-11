#include "EventManager.h"


std::unordered_map<std::string, std::vector<EventManager::EventCallback>> EventManager::events; // Définition ici !

void EventManager::RegisterEvent(const std::string& name, EventCallback function) 
{
    events[name].push_back(function);
}

void EventManager::WriteInt(int value)
{
    // Convert int to 4-byte array (big-endian)
    uint8_t byteArray[4];
    byteArray[0] = static_cast<uint8_t>((value >> 24) & 0xFF); // Extract the most significant byte
    byteArray[1] = static_cast<uint8_t>((value >> 16) & 0xFF);
    byteArray[2] = static_cast<uint8_t>((value >> 8) & 0xFF);
    byteArray[3] = static_cast<uint8_t>(value & 0xFF); // Extract the least significant byte

    // Add each byte to the event's buffer
    for (int i = 0; i < 4; ++i)
    {
        // Store the byte, assuming you have an appropriate storage mechanism (like a vector)
        std::cout << "Byte " << i << ": " << static_cast<int>(byteArray[i]) << std::endl;
        // You would typically store it here, e.g., in a message buffer or send it over the network.
    }
}

void EventManager::ReadInt(int value)
{
    // Assuming you're receiving a byte array of size 4
    uint8_t byteArray[4];  // You should populate this byte array with data from the received packet

    // For illustration purposes, assuming you fill byteArray with actual data here.

    value = 0;
    value |= static_cast<int>(byteArray[0]) << 24; // Combine the bytes back into the integer
    value |= static_cast<int>(byteArray[1]) << 16;
    value |= static_cast<int>(byteArray[2]) << 8;
    value |= static_cast<int>(byteArray[3]);

    std::cout << "Read value: " << value << std::endl;
}

void EventManager::Deserialize(const std::vector<uint8_t>& buffer)
{
    // Initialiser un index pour suivre la position dans le buffer
    int index = 0;

    // Déclarer des variables pour les paramètres
    int param1 = 0;
    int param2 = 0;

    // Lire le premier int (4 bytes)
    memcpy(&param1, &buffer[index], sizeof(int));
    index += sizeof(int);  // Déplacer l'index de 4 bytes

    // Lire le deuxième int (4 bytes)
    memcpy(&param2, &buffer[index], sizeof(int));
    index += sizeof(int);  // Déplacer l'index de 4 bytes

    // Afficher les valeurs pour vérifier
    std::cout << "Premier int: " << param1 << std::endl;
}


