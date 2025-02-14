#include "Message.h"
#include "../Server/ClientManager.h"


struct CreateClientMessage : public Message
{
    static constexpr int ID = 1;

    std::string ipAddress;

    int getId() const override { return ID; }

    std::vector<uint8_t>& serialize(std::vector<uint8_t>& buffer) const override
    {
        buffer.clear();  // Vider le buffer avant de sérialiser

        // Sérialiser l'ID (en 4 octets)
        int id = getId();
        buffer.resize(sizeof(id) + ipAddress.size() + 1);  // +1 pour le caractère '\0'

        std::memcpy(buffer.data(), &id, sizeof(id));  // Copier l'ID dans le buffer

        // Sérialiser l'adresse IP (chaîne de caractères avec '\0' à la fin)
        std::memcpy(buffer.data() + sizeof(id), ipAddress.c_str(), ipAddress.size() + 1);  // Inclut '\0' de fin de chaîne

        return buffer;
    }


    void deserialize(const std::vector<uint8_t>& buffer) override
    {
        // Désérialisation de l'entier (qui est au début du buffer)
        int value;
        std::memcpy(&value, buffer.data(), sizeof(int));

        // Désérialisation de l'adresse IP (à partir de la position après l'entier)
        std::string ipAddress;
        size_t offset = sizeof(int);  // L'adresse IP commence après l'entier
        for (size_t i = offset; i < buffer.size(); ++i) {
            if (buffer[i] == '\0') {
                break;  // Fin de la chaîne
            }
            ipAddress.push_back(static_cast<char>(buffer[i]));
        }

        // Ajouter l'adresse IP au ClientManager
       // ClientManager::addClient(ipAddress);

        // Affichage pour le débogage
        std::cout << "[CreateClientMessage] Received: " << std::endl;
        std::cout << "Value: " << value << ", IP Address: " << ipAddress << std::endl;
    }


    void process() const override
    {
       /* ClientManager::addClient(ipAddress);*/
        std::cout << "[CreateClientMessage] Received: " << std::endl;
    }
};