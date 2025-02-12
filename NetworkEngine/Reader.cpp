#include "Reader.h"

Reader::Reader(const uint8_t* buffer, long bytesAvailable)
    : Buffer(buffer), BytesAvailable(bytesAvailable) {}

int Reader::ReadInt() {
    if (Position + sizeof(int) <= BytesAvailable) {
        int value = *reinterpret_cast<const int*>(Buffer + Position);
        Position += sizeof(int);
        return value;
    }
    else {
        // Gérer l'erreur si les bytes disponibles ne suffisent pas
        // Par exemple, lancer une exception ou retourner une valeur par défaut
        throw std::out_of_range("Not enough bytes available to read an int.");
    }
}
