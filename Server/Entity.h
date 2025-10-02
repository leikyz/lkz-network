#include <cstdint>

enum EntityEnum
{
    Olise = 1,
    Elisa
};

struct Entity
{
    uint32_t m_id;   
    EntityEnum m_type;   

    float m_posX;
    float m_posY;
    float m_posZ;

    Entity(EntityEnum type)
        : m_type(type)
    {
    }

    EntityEnum getType() const { return m_type; }
    void setType(EntityEnum t) { m_type = t; }
};
