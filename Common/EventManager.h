#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include <unordered_map>
#include <vector>
#include <functional>
#include <string>


class EventManager
{
public:
    using MessageHandler = void(*)(const std::vector<uint8_t>&);

    static std::vector<uint8_t> serialize(int value);
    static std::vector<std::string> deserialize(const std::vector<uint8_t>& data);

    template<typename T>
    static void registerHandler(int id) {
        if (id >= 0 && id < 256) {
            messageHandlers[id] = &handleMessage<T>;
        }
    }

    static void BindEvents();

    static void processMessage(std::vector<uint8_t>& buffer) {
        if (buffer.empty()) {
            // std::cerr << "Empty buffer received" << std::endl;
            return;
        }

        // Lire l'ID du message
        int id = static_cast<int>(buffer[0]);

        // Vérifier si l'ID est valide et si un handler est enregistré
        if (id < 0 || id >= 256 || !messageHandlers[id]) {
            // std::cerr << "Unknown message ID: " << id << std::endl;
            return;
        }

        // Supprimer l'ID du buffer (le premier élément)
        buffer.erase(buffer.begin());

        // Appeler le handler avec le buffer restant
        messageHandlers[id](buffer);
    }


private:
    static MessageHandler messageHandlers[256];

    template<typename T>
    static void handleMessage(const std::vector<uint8_t>& buffer) {
        T msg;
        msg.deserialize(buffer);
       /* msg.process();*/
    }
};

#endif // EVENT_MANAGER_H