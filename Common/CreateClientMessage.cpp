#include "Message.h"
#include "../Server/ClientManager.h"

struct CreateClientMessage : public Message
{
    static constexpr int ID = 1;

    std::string ipAddress;
    uint16_t port;

    int getId() const override { return ID; }

    std::vector<uint8_t>& serialize(std::vector<uint8_t>& buffer) const override
    {
        buffer.clear();

        //int id = getId();
        //uint16_t networkPort = htons(port);

        //// Convert SOCKET to uintptr_t to avoid issues with 64-bit/32-bit mismatches
        //uintptr_t socketValue = reinterpret_cast<uintptr_t>(socket);

        //// Calculate the buffer size to hold id, port, socket value, and ipAddress
        //buffer.resize(sizeof(id) + sizeof(networkPort) + sizeof(socketValue) + ipAddress.size() + 1);

        //// Serialize the data into the buffer
        //std::memcpy(buffer.data(), &id, sizeof(id));
        //std::memcpy(buffer.data() + sizeof(id), &networkPort, sizeof(networkPort));
        //std::memcpy(buffer.data() + sizeof(id) + sizeof(networkPort), &socketValue, sizeof(socketValue));
        //std::memcpy(buffer.data() + sizeof(id) + sizeof(networkPort) + sizeof(socketValue), ipAddress.c_str(), ipAddress.size() + 1);

        return buffer;
    }

    void deserialize(const std::vector<uint8_t>& buffer) override
    {
        //int value;
        //uint16_t networkPort;
        //uintptr_t socketValue;

        //// Deserialize the data from the buffer
        //std::memcpy(&value, buffer.data(), sizeof(int));
        //std::memcpy(&networkPort, buffer.data() + sizeof(int), sizeof(networkPort));
        //std::memcpy(&socketValue, buffer.data() + sizeof(int) + sizeof(networkPort), sizeof(socketValue));

        //// Convert back the port and the socket value
        //port = ntohs(networkPort);

        //// Deserialize the IP address
        //ipAddress.clear();
        //size_t offset = sizeof(int) + sizeof(networkPort) + sizeof(socketValue);
        //for (size_t i = offset; i < buffer.size(); ++i) {
        //    if (buffer[i] == '\0') break;
        //    ipAddress.push_back(static_cast<char>(buffer[i]));
        //}
    }

    void process() const override
    {
        // Process logic (not implemented here, as per your comment)
        // Example: Add client to ClientManager
   /*     sockaddr_in clientAddr{};
        clientAddr.sin_family = AF_INET;
        clientAddr.sin_port = htons(port);
        inet_pton(AF_INET, ipAddress.c_str(), &clientAddr.sin_addr);

        ClientManager::addClient(clientAddr, ipAddress);

        std::cout << "[CreateClientMessage] Client added: " << ipAddress << ":" << port << " (Socket: " << socket << ")" << std::endl;*/
    }
};
