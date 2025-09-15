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
    std::list<Client*> clients;  
    std::list<Entity*> entities;  
    int nextEntityId = 1;  

    Lobby(int lobbyId) : id(lobbyId) {}

    void addClient(Client* client) {
        clients.push_back(client);
        std::cout << "Client " << client->ipAddress << " added to lobby: " << id << std::endl;
    }

    void addEntity(Entity* entity) {
        entity->id = nextEntityId++;  
        entities.push_back(entity);
        std::cout << "Entity with ID " << entity->id << " added to lobby: " << id << std::endl;
    }

    void removeEntity(uint32_t entityId) {
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
