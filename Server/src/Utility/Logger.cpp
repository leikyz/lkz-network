#include "LKZ/Utility/Logger.h"
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
	case LogType::Received:return "[RECV] ";
	case LogType::Sent:    return "[SENT] ";
    default:               return "";
    }
}

void Logger::Log(const std::string& message, LogType level)
{
    auto task = [msg = message, level]() {
        auto now = std::chrono::system_clock::now();
        std::time_t time = std::chrono::system_clock::to_time_t(now);
        std::tm timeinfo{};
        localtime_s(&timeinfo, &time);
        char buffer[9];
        strftime(buffer, sizeof(buffer), "%H:%M:%S", &timeinfo);

        std::cout << "[" << buffer << "] " << Logger::GetPrefix(level) << msg << std::endl;

        // Add a short pause to slow log printing
        //std::this_thread::sleep_for(std::chrono::milliseconds(50)); // 50ms pause
        };

    auto loggerPool = ThreadManager::GetPool("logger");
    if (loggerPool) {
        loggerPool->EnqueueTask(task);
    }
    else {
        std::cerr << "[Logger] Logger pool not found!\n";
    }
}


