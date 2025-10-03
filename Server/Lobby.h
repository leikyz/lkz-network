#ifndef LOBBY_H
#define LOBBY_H

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
    byte mapId;
    std::vector<Client*> clients;  
    std::vector<Entity*> entities;
    int nextEntityId = 1;  

    Lobby(int lobbyId) : id(lobbyId) {}

    void addClient(Client* client) {
        clients.push_back(client);
        std::cout << "Client " << client->ipAddress << " added to lobby: " << id << std::endl;
    }

	/**
	* @brief Add an entity to the lobby and assign it a unique ID.
	* @param Entity Pointer to the entity to be added.
    **/

    void addEntity(Entity* entity) {
        entity->m_id = m_nextEntityId++;  
        m_entities.push_back(entity);
        std::cout << "Entity with ID " << entity->m_id << " added to lobby: " << m_id << std::endl;
    }

    void removeEntity(uint32_t entityId) {
        for (auto it = entities.begin(); it != entities.end(); ) {
            if ((*it)->id == entityId) {
                delete* it;              
                it = m_entities.erase(it); 
                std::cout << "Entity with ID " << entityId << " removed from lobby: " << m_id << std::endl;
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

    std::list<Client*> getClients() const
    {
        return clients;
    }

    std::list<Entity*> getEntities() const
    {
        return entities;
    }

    Entity* getEntityById(uint32_t entityId) const
    {
        for (const auto& entity : m_entities)
        {
            if (entity->m_id == entityId)
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

#endif // LOBBY_H
