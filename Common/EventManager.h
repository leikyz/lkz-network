#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include <unordered_map>
#include <vector>
#include <functional>
#include <string>

class EventManager
{
public:
    static std::vector<int8_t> Serialize(int value);
    static std::vector<std::string> Deserialize(const std::vector<int8_t>& data);

private:

};

#endif // EVENT_MANAGER_H