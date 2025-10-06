#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <vector>
#include <cstdint>

class Serializer {
private :
    std::vector<uint8_t> m_buffer;
public:
    
    void writeInt(int value) {
        m_buffer.push_back(static_cast<uint8_t>(value & 0xFF));
        m_buffer.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
        m_buffer.push_back(static_cast<uint8_t>((value >> 16) & 0xFF));
        m_buffer.push_back(static_cast<uint8_t>((value >> 24) & 0xFF));
    }

    void writeFloat(float value) {
        uint8_t bytes[sizeof(float)];
        std::memcpy(bytes, &value, sizeof(float)); 
        m_buffer.insert(m_buffer.end(), bytes, bytes + sizeof(float));
    }

    void writeBool(bool value) {
        m_buffer.push_back(value ? 1 : 0);
    }

    void writeByte(uint8_t value) {
        m_buffer.push_back(value);
    }

    std::vector<uint8_t>& getBuffer() {
        return m_buffer;
	}

    void reset() {
        m_buffer.clear();
	}
};

#endif // SERIALIZER_H
