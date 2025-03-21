#include <cstdint>

enum EntityEnum
{
    Olise = 1,
    Elisa
};

struct Entity
{
    uint32_t id;   
    EntityEnum type;   

    float posX;
    float posY;
    float posZ;

    Entity(EntityEnum type)
        : type(type)
    {
    }

    EntityEnum getType() const { return type; }
    void setType(EntityEnum t) { type = t; }
};
