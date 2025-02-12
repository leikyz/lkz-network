#pragma once
#include <cstdint>
#include <stdexcept>  // Pour std::out_of_range

class Reader {
public:
    long Position = 0;
    long BytesAvailable = 0;
    const uint8_t* Buffer = nullptr;

    Reader(const uint8_t* buffer, long bytesAvailable);

    int ReadInt();
};
