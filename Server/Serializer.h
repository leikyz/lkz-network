#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <vector>
#include <cstdint>

class Serializer {
public:
    std::vector<uint8_t> buffer;

    void writeInt(int value) {
        buffer.push_back(static_cast<uint8_t>(value & 0xFF));
        buffer.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
        buffer.push_back(static_cast<uint8_t>((value >> 16) & 0xFF));
        buffer.push_back(static_cast<uint8_t>((value >> 24) & 0xFF));
    }

    void writeFloat(float value) {
        uint8_t bytes[sizeof(float)];
        std::memcpy(bytes, &value, sizeof(float)); 
        buffer.insert(buffer.end(), bytes, bytes + sizeof(float));
    }
};

#endif // SERIALIZER_H
