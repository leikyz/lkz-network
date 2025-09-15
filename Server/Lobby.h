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
    int id;
    byte mapId;
    std::list<std::shared_ptr<Client>> clients;  
    std::list<std::shared_ptr<Entity>> entities;  
    int nextEntityId = 1;  

    Lobby(int lobbyId) : id(lobbyId) {}

    void addClient(const std::shared_ptr<Client>& client) {
        clients.push_back(client);
        std::cout << "Client " << client->ipAddress << " added to lobby: " << id << std::endl;
    }

    void addEntity(const std::shared_ptr<Entity>& entity) {
        entity->id = nextEntityId++;  
        entities.push_back(entity);
        std::cout << "Entity with ID " << entity->id << " added to lobby: " << id << std::endl;
    }

    void removeEntity(uint32_t entityId) {
        entities.remove_if([entityId](const std::shared_ptr<Entity>& entity) {
            return entity->id == entityId;
            });
        std::cout << "Entity with ID " << entityId << " removed to lobby: " << id << std::endl;
    }

    std::shared_ptr<Client> getClientByIp(const std::string& ipAddress) const
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

    std::list<std::shared_ptr<Client>> getClients() const
    {
        return clients;
    }

    std::list<std::shared_ptr<Entity>> getEntities() const
    {
        return entities;
    }

    std::shared_ptr<Entity> getEntityById(uint32_t entityId) const
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

};

#endif // LOBBY_H
