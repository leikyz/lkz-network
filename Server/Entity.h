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
    float posX;
    float posY;
    float posZ;

    float rotaX;
    float rotaY;
    float rotaZ;

    // Constructeur
    Entity(EntityEnum type)
        : type(type)
    {
    }

    EntityEnum getType() const { return type; }
    void setType(EntityEnum t) { type = t; }
};
