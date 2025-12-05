#include <unordered_map>
#include <memory>
#include <string>
#include <iostream>
#include <list>
#include "LKZ/Core/ECS/Entity.h" 
#include "LKZ/Core/Manager/ClientManager.h"  
#include "LKZ/Utility/Logger.h"
#pragma once
/**
* @brief Represents a game lobby where clients and entities are managed.
*/
enum class LobbyState {
    WAITING,    // En attente de joueurs / Ready
    STARTING,   // Le décompte de 5s
    IN_GAME,    // La partie tourne
    ENDING      // Résumé des scores
};

struct Lobby
{
    LobbyState state = LobbyState::WAITING;
    static constexpr int MAX_PLAYER = 4;
    int id;
    uint8_t mapId;
    std::vector<Client*> clients;
    int nextEntityId = 1;
	bool inGame = false;

    bool isStarting = false;
    float startTimer = 0.0f;
    const float TIME_BEFORE_FIRST_WAVE = 5.0f;

   /* Wave wave;*/
    /**
    * @brief Constructor for Lobby.
    * @param lobbyId Unique identifier for the lobby.
    */

    Lobby(int lobbyId,uint8_t mapId) : id(lobbyId), mapId(mapId) {
        clients.reserve(MAX_PLAYER);  // Prevents reallocations
    }

    /**
    * @brief Add a client to the lobby.
    * @param client Pointer to the client to be added.
    */

    void addClient(Client* client)
    {
        clients.push_back(client);
		Logger::Log("Client " + client->ipAddress + " added to lobby: " + std::to_string(id), LogType::Info);
    }

    /**
    * @brief Add an entity to the lobby and assign it a unique ID.
    * @param Entity Pointer to the entity to be added.
    **/

    void addEntity(Entity* entity) {
      /*  entity->id = nextEntityId++;
        entities.push_back(entity);
		Logger::Log("Entity with ID " + std::to_string(entity->id) + " added to lobby: " + std::to_string(id), LogType::Info);*/
    }

    /**
    * @brief Remove an entity from the lobby by its ID.
    * @param EntityId Unique identifier of the entity to be removed.
    */

    void removeEntity(uint32_t entityId)
    {
       /* for (auto it = entities.begin(); it != entities.end(); ) {
            if ((*it)->id == entityId) {
                delete* it;
                it = entities.erase(it);
				Logger::Log("Entity with ID " + std::to_string(entityId) + " removed from lobby: " + std::to_string(id), LogType::Info);
            }
            else {
                ++it;
            }
        }*/
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

    /*const std::vector<Entity*>& getEntities() const { return entities; }*/

    Entity* getEntityById(uint32_t entityId) const
    {
       /* for (const auto& entity : entities)
        {
            if (entity->id == entityId)
            {
                return entity;
            }
        }*/
        return nullptr;
    }

    int getClientCount() const
    {
        return static_cast<int>(clients.size());
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


    //int CalculateZombiesForWave(Wave& settings) {
    //    // Formule : (Base * (Multi ^ Wave)) + (Bonus par joueur)
    //    int total = (int)(settings.baseZombies * std::pow(settings.multiplier, settings.waveNumber - 1));

    //    // Ajout pour le multijoueur (ex: +30% de zombies par joueur supplémentaire)
    //    if (settings.playerCount > 1) {
    //        total += (int)(total * 0.3f * (settings.playerCount - 1));
    //    }
    //    return total;
    //}


};

