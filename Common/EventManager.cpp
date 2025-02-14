#include "EventManager.h"
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdint>
#include "CreateClientMessage.cpp"

EventManager::MessageHandler EventManager::messageHandlers[256] = { nullptr };

std::vector<uint8_t> EventManager::serialize(int value) {
    std::vector<uint8_t> serialized(sizeof(int));
    std::memcpy(serialized.data(), &value, sizeof(int));
    return serialized;
}

std::vector<std::string> EventManager::deserialize(const std::vector<uint8_t>& data) {
    std::vector<std::string> results;

    if (data.size() >= sizeof(int)) {
        int value = 0;
        std::memcpy(&value, data.data(), sizeof(int));
        std::cout << "Deserialized int: " << value << std::endl;
        results.push_back(std::to_string(value));
    }
    else {
        std::cerr << "Invalid data size for deserialization." << std::endl;
    }

    return results;
}

void EventManager::BindEvents()
{
    EventManager::registerHandler<CreateClientMessage>(1);
}
