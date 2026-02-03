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
#define LOG_LOCATION(level, file, line) Utils::Logger::GetInstance() << "[" << Utils::Logger::GetInstance().SetLogLevel(level) << "][" << file << ":" << line << "] " << Utils::Logger::GetInstance().CurrentTime() << ": "
// #define LOG_ASSERT_ON_ERROR

#define GL_CHECK() Utils::checkOpenGLError(__FILE__, __LINE__)
#define GL_FCHECK(func) func; Utils::checkOpenGLError(__FILE__, __LINE__)

#define LOG_INIT(filename) Utils::Logger::GetInstance().SetLogFile(filename)
#define LOG_CLOSE Utils::Logger::GetInstance().CloseLogFile()
#define LOG_SET_PRINT_TO_CONSOLE(value) Utils::Logger::GetInstance().SetPrintToConsole(value)
#define LOG_WRITE() Utils::Logger::GetInstance().WriteLogFile()

#define LOG_CONTENTS() Utils::Logger::GetInstance().GetLogContents()
#define LOG_LINE_LEVELS() Utils::Logger::GetInstance().GetLineLogLevels()

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
        std::string filename;
        std::stringstream logContents;
        std::vector<LogLevel> lineLogLevels = std::vector<LogLevel>();
        LogLevel logLevel;
        bool printToConsole;

    public:
        Logger() {}
        Logger(Logger const&)          = delete;
        void operator=(Logger const&)  = delete;

        ~Logger()
        {
            WriteLogFile();
        }

        static Logger& GetInstance()
        {
            static Logger instance;
            return instance;
        }

        void SetLogFile(const std::string& filename) { this->filename = filename; }

        void WriteLogFile()
        {
            OpenLogFile();
            logFile << logContents.str();
            CloseLogFile();
        }

        void OpenLogFile()
        {
            logFile.open(filename, std::ios::out | std::ios::trunc);
            if (!logFile)
            {
                std::cerr << "Failed to open log file: " << filename << std::endl;
            }
        }

        void CloseLogFile()
        {
            if (logFile.is_open())
                logFile.close();
        }



        void SetPrintToConsole(bool value) { printToConsole = value; }

        template <typename T>
        Logger& operator<<(const T& data)
        {
            logContents << data;
            if (printToConsole)
            {
                if (logLevel == LogLevel::ERROR)
                {
                    std::cerr << data;
                }
                else
                {
                    std::cout << data;
                }
            }
            return *this;
        }

        std::string GetLogContents() { return logContents.str(); }
        std::vector<LogLevel> GetLineLogLevels() { return lineLogLevels; }

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
            lineLogLevels.push_back(logLevel);
            switch (level)
            {
            case LogLevel::INFO: return "INFO";
            case LogLevel::WARNING: return "WARNING";
            case LogLevel::ERROR: return "ERROR";
            default: return "UNKNOWN";
            }
        }
    };

    inline void checkOpenGLError(const char* file, int line)
    {
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            switch (err) {
            case GL_INVALID_ENUM:
                LOG_LOCATION(LOG_ERROR, file, line) << "GL_INVALID_ENUM: An unacceptable value was specified for an enumerated argument." << "\n";
                break;
            case GL_INVALID_VALUE:
                LOG_LOCATION(LOG_ERROR, file, line) << "GL_INVALID_VALUE: A numeric argument is out of range." << "\n";
                break;
            case GL_INVALID_OPERATION:
                LOG_LOCATION(LOG_ERROR, file, line) << "GL_INVALID_OPERATION: The specified operation is not allowed in the current state." << "\n";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                LOG_LOCATION(LOG_ERROR, file, line) << "GL_INVALID_FRAMEBUFFER_OPERATION: The framebuffer object is not complete." << "\n";
                break;
            case GL_OUT_OF_MEMORY:
                LOG_LOCATION(LOG_ERROR, file, line) << "GL_OUT_OF_MEMORY: There is not enough memory left to execute the command." << "\n";
                break;
            default:
                LOG_LOCATION(LOG_ERROR, file, line) << "Unknown OpenGL error: " << err << "\n";
            }
        }
    }
}
