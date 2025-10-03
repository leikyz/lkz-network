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

    /**
     * @brief Constructor for Entity.
     * @param t Type of the entity.
     * @param id Unique identifier for the entity (default is 0).
     * @param x X position (default is 0).
     * @param y Y position (default is 0).
     * @param z Z position (default is 0).
	 */ 
    Entity(EntityEnum t, uint32_t id = 0, float x = 0, float y = 0, float z = 0)
        : id(id), posX(x), posY(y), posZ(z), type(t)
    {
    }

    EntityEnum getType() const { return type; }
    void setType(EntityEnum t) { type = t; }
};
