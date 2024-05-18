#pragma once

#include <iostream>
#include <chrono>
#include <ctime>
#include <fstream>
#include <regex>
#include "ClassName.h"

// TODO: I have no clue if this will work or not...
// TODO: Fix

#define __CLASSNAME__ type_name<decltype(*this)>()

// LOG macro adds log level, class name, and date boilerplate to beginning of log message
#define LOG(logger, level) logger << "[" << logger.SetLogLevel(level) << "][" <<  __CLASSNAME__ << "] " << logger.CurrentTime() << ": "

#define LOG_INFO Utils::LogLevel::INFO
#define LOG_WARNING Utils::LogLevel::WARNING
#define LOG_ERROR Utils::LogLevel::ERROR

namespace Utils
{
    enum class LogLevel
    {
        INFO,
        WARNING,
        ERROR
    };

    class Logger
    {
        std::ofstream logFile;
        Utils::LogLevel logLevel;
        bool printToConsole;

    public:
        Logger(const std::string& filename, bool printToConsole = true)
            : logFile(filename, std::ios::out | std::ios::trunc), printToConsole(printToConsole)
        {
            if (!logFile)
            {
                std::cerr << "Failed to open log file: " << filename << std::endl;
            }
        }

        ~Logger()
        {
            logFile.close();
        }

        template <typename T>
        Logger& operator<<(const T& data)
        {
            logFile << data;
            if (printToConsole)
            {
                if (logLevel == LogLevel::ERROR)
                    std::cerr << data;
                else
                    std::cout << data;
            }
            return *this;
        }

        static std::string CurrentTime()
        {
            auto now = std::chrono::system_clock::now();
            auto in_time_t = std::chrono::system_clock::to_time_t(now);

            std::stringstream ss;
            ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");

            return ss.str();
        }
        std::string SetLogLevel(const LogLevel level)
        {
            logLevel = level;
            switch (level)
            {
            case LogLevel::INFO: return "INFO";
            case LogLevel::WARNING: return "WARNING";
            case LogLevel::ERROR: return "ERROR";
            default: return "UNKNOWN";
            }
        }
    };
}
