#ifndef LOBBY_H
#define LOBBY_H

#include <unordered_map>
#include <memory>
#include <string>
#include <iostream>
#include <list>
#include "ClientManager.h"  
#include "Entity.h" 

struct Lobby
{
    static constexpr int MAX_PLAYER = 4;
    int m_id;
    byte m_mapId;
    std::list<Client*> m_clients;  
    std::list<Entity*> m_entities;  
    int m_nextEntityId = 1;  

    Lobby(int lobbyId) : m_id(lobbyId) {}

    void addClient(Client* client) {
        m_clients.push_back(client);
        std::cout << "Client " << client->m_ipAddress << " added to lobby: " << m_id << std::endl;
    }

    void addEntity(Entity* entity) {
        entity->m_id = m_nextEntityId++;  
        m_entities.push_back(entity);
        std::cout << "Entity with ID " << entity->m_id << " added to lobby: " << m_id << std::endl;
    }

    void removeEntity(uint32_t entityId) {
        for (auto it = m_entities.begin(); it != m_entities.end(); ) {
            if ((*it)->m_id == entityId) {
                delete* it;              
                it = m_entities.erase(it); 
                std::cout << "Entity with ID " << entityId << " removed from lobby: " << m_id << std::endl;
            }
            else {
                ++it;
            }
        }
    }


    Client* getClientByIp(const std::string& ipAddress) const
    {
        for (const auto& client : m_clients)
        {
            if (client->m_ipAddress == ipAddress)
            {
                return client;
            }
        }
        return nullptr;
    }

    std::list<Client*> getClients() const
    {
        return m_clients;
    }

    std::list<Entity*> getEntities() const
    {
        return m_entities;
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

};

#endif // LOBBY_H
