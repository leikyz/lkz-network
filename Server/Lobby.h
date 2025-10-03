#include <unordered_map>
#include <memory>
#include <string>
#include <iostream>
#include <list>
#include "ClientManager.h"  
#include "Entity.h" 

/**
* @brief Represents a game lobby where clients and entities are managed.
*/
struct Lobby
{
    static constexpr int MAX_PLAYER = 4;
    int id;
    uint8_t mapId;
    std::vector<Client*> clients;
    std::vector<Entity*> entities;
    int nextEntityId = 1;

    /**
    * @brief Constructor for Lobby.
    * @param lobbyId Unique identifier for the lobby.
    */

    Lobby(int lobbyId,uint8_t mapId) : id(lobbyId) {
        clients.reserve(MAX_PLAYER);  // Prevents reallocations
    }

    /**
    * @brief Add a client to the lobby.
    * @param client Pointer to the client to be added.
    */

    void addClient(Client* client)
    {
        clients.push_back(client);
        std::cout << "Client " << client->ipAddress << " added to lobby: " << id << std::endl;
    }

    /**
    * @brief Add an entity to the lobby and assign it a unique ID.
    * @param Entity Pointer to the entity to be added.
    **/

    void addEntity(Entity* entity) {
        entity->id = nextEntityId++;
        entities.push_back(entity);
        std::cout << "Entity with ID " << entity->id << " added to lobby: " << id << std::endl;
    }

    /**
    * @brief Remove an entity from the lobby by its ID.
    * @param EntityId Unique identifier of the entity to be removed.
    */

    void removeEntity(uint32_t entityId)
    {
        for (auto it = entities.begin(); it != entities.end(); ) {
            if ((*it)->id == entityId) {
                delete* it;
                it = entities.erase(it);
                std::cout << "Entity with ID " << entityId << " removed from lobby: " << id << std::endl;
            }
            else {
                ++it;
            }
        }
    }

    /// <summary>
    /// Remove client from lobby
    /// </summary>
    /// <param name="client"></param>

    void removeClient(Client* client)
    {
        clients.erase(std::remove(clients.begin(), clients.end(), client), clients.end());
    }

    // Getters

    const std::vector<Client*>& getClients() const { return clients; }

    const std::vector<Entity*>& getEntities() const { return entities; }

    Entity* getEntityById(uint32_t entityId) const
    {
        for (const auto& entity : entities)
        {
            if (entity->id == entityId)
            {
                return entity;
            }
        }
        return nullptr;
    }

    Client* getClientByIp(const std::string& ipAddress) const
    {
        for (const auto& client : clients)
        {
            if (client->ipAddress == ipAddress)
            {
                return client;
            }
        }
        return nullptr;
    }


};

