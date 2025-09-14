#ifndef DESERIALIZER_H
#define DESERIALIZER_H

#include <vector>
#include <cstdint>
#include <stdexcept>

class Deserializer {
private:
    const std::vector<uint8_t>& buffer;
    size_t position = 0;

public:
    explicit Deserializer(const std::vector<uint8_t>& buf) : buffer(buf) {}

    int readInt() 
    {
        if (position + 4 > buffer.size()) {
            throw std::out_of_range("Buffer overflow in readInt");
        }

        int value = buffer[position] |
            (buffer[position + 1] << 8) |
            (buffer[position + 2] << 16) |
            (buffer[position + 3] << 24);

        position += 4;
        return value;
    }

    float readFloat()
    {
        if (position + sizeof(float) > buffer.size()) {
            throw std::out_of_range("Buffer overflow in readFloat");
        }

        float value;
        std::memcpy(&value, &buffer[position], sizeof(float));
        position += sizeof(float);
        return value;
    }

    bool readBool()
    {
        if (position + 1 > buffer.size()) {
            throw std::out_of_range("Buffer overflow in readBool");
        }

        bool value = buffer[position] != 0;
        position += 1;
        return value;
    }

    uint8_t readByte()
    {
        if (position + 1 > buffer.size()) {
            throw std::out_of_range("Buffer overflow in readByte");
        }

        uint8_t value = buffer[position];
        position += 1;
        return value;
    }

};

#endif // DESERIALIZER_H
