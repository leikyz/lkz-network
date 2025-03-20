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
    int id;
    std::list<std::shared_ptr<Client>> clients;  
    std::list<std::shared_ptr<Entity>> entities;  
    int nextEntityId = 1;  

    // Constructeur
    Lobby(int lobbyId) : id(lobbyId) {}

    // Ajouter un client au lobby
    void addClient(const std::shared_ptr<Client>& client) {
        clients.push_back(client);
        std::cout << "Client " << client->ipAddress << " ajouté au lobby: " << id << std::endl;
    }

    // Ajouter une entité au lobby
    void addEntity(const std::shared_ptr<Entity>& entity) {
        entity->id = nextEntityId++;  
        entities.push_back(entity);
        std::cout << "Entité avec ID " << entity->id << " ajoutée au lobby: " << id << std::endl;
    }

    // Supprimer une entité par son ID
    void removeEntity(uint32_t entityId) {
        entities.remove_if([entityId](const std::shared_ptr<Entity>& entity) {
            return entity->id == entityId;
            });
        std::cout << "Entité avec ID " << entityId << " supprimée du lobby: " << id << std::endl;
    }

    // Récupérer un client par adresse IP
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

    // Récupérer la liste des clients
    std::list<std::shared_ptr<Client>> getClients() const
    {
        return clients;
    }

    // Récupérer la liste des entités
    std::list<std::shared_ptr<Entity>> getEntities() const
    {
        return entities;
    }
};

#endif // LOBBY_H
