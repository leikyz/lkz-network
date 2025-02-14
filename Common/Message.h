#include <vector>
#include <iostream>

struct Message
{
    virtual int getId() const = 0;

    virtual std::vector<uint8_t>& serialize(std::vector<uint8_t>& buffer) const = 0;
    virtual void deserialize(const std::vector<uint8_t>& buffer) = 0;
    virtual void process() const = 0;

    virtual ~Message() = default;

};