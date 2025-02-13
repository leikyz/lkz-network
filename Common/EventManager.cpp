#include "EventManager.h"
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdint>

// Serialize un int en bytes
std::vector<int8_t> EventManager::Serialize(int value) {
    std::vector<int8_t> serialized(sizeof(int));

    // Copier les bytes de l'int dans le tableau
    std::memcpy(serialized.data(), &value, sizeof(int));

    return serialized;
}

// Deserialize les bytes en int et affiche le r�sultat
std::vector<std::string> EventManager::Deserialize(const std::vector<int8_t>& data) {
    std::vector<std::string> results;

    if (data.size() >= sizeof(int)) {
        int value = 0;
        std::memcpy(&value, data.data(), sizeof(int));

        // Affiche la valeur d�s�rialis�e
        std::cout << "Deserialized int: " << value << std::endl;

        results.push_back(std::to_string(value));
    }
    else {
        std::cerr << "Invalid data size for deserialization." << std::endl;
    }

    return results;
}