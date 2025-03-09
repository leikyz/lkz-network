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

        buffer.push_back(static_cast<uint8_t>(ID & 0xFF));
        buffer.push_back(static_cast<uint8_t>((ID >> 8) & 0xFF));
        buffer.push_back(static_cast<uint8_t>((ID >> 16) & 0xFF));
        buffer.push_back(static_cast<uint8_t>((ID >> 24) & 0xFF));

        buffer.push_back(isCreated ? 1 : 1);

        return buffer;
    }

    void deserialize(const std::vector<uint8_t>& buffer) override
    {
        if (buffer.size() >= 1)
        {
            isCreated = (buffer[0] == 1);
        }
        else
        {
            std::cerr << "Error: Buffer size too small for deserialization.\n";
        }
    }

    void process(const sockaddr_in& senderAddr) const override
    {
        CreateClientMessage message(isCreated);
        Server::Send(senderAddr, message);
    }
};
