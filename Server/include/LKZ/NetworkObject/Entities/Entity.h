#include <cstdint>
/**
 * @brief Enum representing different types of entities.
 */
enum EntityEnum : uint8_t
{
    Olise = 1,
    Elisa
};

/**
 * @brief Represents a game entity in the lobby.
 */
struct Entity
{
    uint32_t id;
    float posX;
    float posY;
    float posZ;
    EntityEnum type;

    Entity(EntityEnum type)
        : type(type)
    {
    }

    EntityEnum getType() const { return type; }
    void setType(EntityEnum t) { type = t; }
};
