#pragma once

#include <iostream>
#include <chrono>
#include <ctime>
#include <fstream>
#include <regex>
#include <iomanip>
#include "ClassName.h"

#ifdef LOG_CLASS_NAME
#define __CLASSNAME__ type_name<decltype(*this)>()
// LOG macro adds log level, class name, and date boilerplate to beginning of log message
#define LOG(level) Utils::Logger::GetInstance() << "[" << Utils::Logger::GetInstance().SetLogLevel(level) << "][" <<  __CLASSNAME__ << "] " << Utils::Logger::GetInstance().CurrentTime() << ": "
#else
#define LOG(level) Utils::Logger::GetInstance() << "[" << Utils::Logger::GetInstance().SetLogLevel(level) << "] " << Utils::Logger::GetInstance().CurrentTime() << ": "
#endif

#define LOG_INIT(filename) Utils::Logger::GetInstance().SetLogFile(filename)
#define LOG_CLOSE Utils::Logger::GetInstance().CloseLogFile()
#define LOG_SET_PRINT_TO_CONSOLE(value) Utils::Logger::GetInstance().SetPrintToConsole(value)

#define LOG_CONTENTS() Utils::Logger::GetInstance().GetLogContents()

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
        std::stringstream logContents;
        std::string filename;
        LogLevel logLevel;
        bool printToConsole;

    public:
        // static std::unique_ptr<Logger> instance;

        Logger() {}
        Logger(Logger const&)          = delete;
        void operator=(Logger const&)  = delete;

        ~Logger()
        {
            CloseLogFile();
        }

        static Logger& GetInstance()
        {
            static Logger instance;
            return instance;
        }

        void SetLogFile(const std::string& filename)
        {
            this->filename = filename;
        }

        void SetPrintToConsole(bool value)
        {
            printToConsole = value;
        }

        void CloseLogFile()
        {
            std::ofstream logFile;
            logFile.open(filename, std::ios::out | std::ios::trunc);
            if (!logFile)
            {
                std::cerr << "Failed to open log file: " << filename << std::endl;
            }
        }

        template <typename T>
        Logger& operator<<(const T& data)
        {
            logContents << data;
            if (printToConsole)
            {
                if (logLevel == LogLevel::ERROR)
                    std::cerr << data;
                else
                    std::cout << data;
            }
            return *this;
        }

        std::string GetLogContents() { return logContents.str(); }

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
