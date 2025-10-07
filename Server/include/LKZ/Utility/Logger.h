#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include "LKZ/Core/Threading/ThreadManager.h"
#include <format>

enum class LogType
{
    Info,
    Warning,
    Error,
    Debug,
    Received,
    Sent
};

class Logger
{
public:
    static void Log(const std::string& message, LogType level = LogType::Info);

private:
    static std::string GetPrefix(LogType level);
};

#endif
