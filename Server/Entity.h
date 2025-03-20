#include <cstdint>

enum EntityEnum
{
    Player = 1,
    Fish
};

struct Entity
{
    uint32_t id;   
    EntityEnum type;   

    // Constructeur
    Entity(EntityEnum type)
        : type(type)
    {
    }

    EntityEnum getType() const { return type; }
    void setType(EntityEnum t) { type = t; }
};
