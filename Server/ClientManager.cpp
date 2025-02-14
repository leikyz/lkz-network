#include "ClientManager.h"

// Définition de la map statique pour stocker les clients
std::unordered_map<int, std::shared_ptr<Client>> ClientManager::clients;

// Ajouter un client avec son adresse IP et son socket
void ClientManager::addClient(const std::string& ipAddress, SOCKET sock)
{
    std::shared_ptr<Client> newClient = std::make_shared<Client>(ipAddress, sock);  // Crée un client avec l'IP et le socket

    // Ajouter le client à la map
    clients[newClient->clientId] = newClient;

    std::cout << "Client ajouté: ID = " << newClient->clientId << ", IP = " << newClient->ipAddress << std::endl;
}

// Rechercher un client par ID
std::shared_ptr<Client> ClientManager::getClientById(int clientId)
{
    auto it = clients.find(clientId);
    if (it != clients.end()) {
        return it->second;
    }
    return nullptr;  // Retourne nullptr si le client n'est pas trouvé
}

// Afficher tous les clients
void ClientManager::displayClients()
{
    for (const auto& pair : clients) {
        std::cout << "Client ID: " << pair.second->clientId << ", IP: " << pair.second->ipAddress << std::endl;
    }
}
