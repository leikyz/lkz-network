#include "Message.h"
#include "../Server/ClientManager.h"

struct CreateClientMessage : public Message
{
    static constexpr int ID = 1;

    int getId() const override { return ID; }

    std::vector<uint8_t>& serialize(std::vector<uint8_t>& buffer) const override
    {
        buffer.clear();


        return buffer;
    }

    void deserialize(const std::vector<uint8_t>& buffer) override
    {

    }

    void process(const sockaddr_in& senderAddr) const override
    {
        std::cout << " Clients added:"<< std::endl;
    }
};
