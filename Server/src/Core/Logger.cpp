#include "LKZ/Core/Logger.h"
#include <iostream>
#include <sstream>
#include <chrono>
#include <iomanip>

std::string Logger::GetPrefix(LogType level)
{
    switch (level)
    {
    case LogType::Info:    return "[INFO] ";
    case LogType::Warning: return "[WARNING] ";
    case LogType::Error:   return "[ERROR] ";
    case LogType::Debug:   return "[DEBUG] ";
    default:               return "";
    }
}

void Logger::Log(const std::string& message, LogType level)
{
    // Capture the message so it can be safely used in a lambda
    auto task = [msg = message, level]() {
        // Add timestamp
        auto now = std::chrono::system_clock::now();
        std::time_t time = std::chrono::system_clock::to_time_t(now);
        std::tm timeinfo{};
        localtime_s(&timeinfo, &time);
        char buffer[9];
        strftime(buffer, sizeof(buffer), "%H:%M:%S", &timeinfo);

        std::cout << "[" << buffer << "] " << Logger::GetPrefix(level) << msg << std::endl;
        };

    // Enqueue into the logger thread pool
    ThreadManager::EnqueueTask("logger", task);
}
