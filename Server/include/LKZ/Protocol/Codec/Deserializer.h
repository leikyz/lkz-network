#ifndef DESERIALIZER_H
#define DESERIALIZER_H

#include <vector>
#include <cstdint>
#include <stdexcept>

class Deserializer {
private:
   
    size_t m_position = 0;

public:
    explicit Deserializer(const std::vector<uint8_t>& buf) : m_buffer(buf) {}
    const std::vector<uint8_t>& m_buffer;
    int readInt() 
    {
        if (m_position + 4 > m_buffer.size()) {
            throw std::out_of_range("Buffer overflow in readInt");
        }

        int value = m_buffer[m_position] |
            (m_buffer[m_position + 1] << 8) |
            (m_buffer[m_position + 2] << 16) |
            (m_buffer[m_position + 3] << 24);

        m_position += 4;
        return value;
    }

    float readFloat()
    {
        if (m_position + sizeof(float) > m_buffer.size()) {
            throw std::out_of_range("Buffer overflow in readFloat");
        }

        float value;
        std::memcpy(&value, &m_buffer[m_position], sizeof(float));
        m_position += sizeof(float);
        return value;
    }

    bool readBool()
    {
        if (m_position + 1 > m_buffer.size()) {
            throw std::out_of_range("Buffer overflow in readBool");
        }

        bool value = m_buffer[m_position] != 0;
        m_position += 1;
        return value;
    }

    uint8_t readByte()
    {
        if (m_position + 1 > m_buffer.size()) {
            throw std::out_of_range("Buffer overflow in readByte");
        }

        uint8_t value = m_buffer[m_position];
        m_position += 1;
        return value;
    }

};

#endif // DESERIALIZER_H
