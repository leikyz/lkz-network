#include "Message.h"
#include "../Server/ClientManager.h"
#include "Server.h"

struct CreateClientMessage : public Message
{
    static constexpr int ID = 1;

    bool isCreated;

    // Constructor to initialize isCreated
    CreateClientMessage(bool isCreated = false) : isCreated(isCreated) {}

    int getId() const override { return ID; }

    std::vector<uint8_t>& serialize(std::vector<uint8_t>& buffer) const override
    {
        buffer.clear();

        // Serialize ID (fixed ID value as a 4-byte integer)
       // Serialize ID in little-endian order
        buffer.push_back(static_cast<uint8_t>(ID & 0xFF));
        buffer.push_back(static_cast<uint8_t>((ID >> 8) & 0xFF));
        buffer.push_back(static_cast<uint8_t>((ID >> 16) & 0xFF));
        buffer.push_back(static_cast<uint8_t>((ID >> 24) & 0xFF));


        //// Serialize isCreated (1 for true, 0 for false)
        buffer.push_back(isCreated ? 1 : 1);

        return buffer;
    }


    // Deserialize the message
    void deserialize(const std::vector<uint8_t>& buffer) override
    {
        if (buffer.size() >= 1)
        {
            // Deserialize the isCreated value (1 means true, 0 means false)
            isCreated = (buffer[0] == 1);
        }
        else
        {
            std::cerr << "Error: Buffer size too small for deserialization.\n";
        }
    }

    // Process the message
    void process(const sockaddr_in& senderAddr) const override
    {
        std::cout << "Client creation status: " << (isCreated ? "Created" : "Not Created") << std::endl;

        // Create a CreateClientMessage instance and send it by reference
        CreateClientMessage message(isCreated);
        Server::Send(senderAddr, message);
    }

};
