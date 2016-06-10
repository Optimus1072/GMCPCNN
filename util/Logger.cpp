//
// Created by wrede on 12.05.16.
//

#include <iostream>
#include "Logger.h"

namespace util
{
    Logger::Logger() : debug_(false), info_(false)
    {
        /* EMPTY */
    }

    void Logger::LogMessage(const std::string& message)
    {
        std::cout << message;
    }

    void Logger::LogInfo(const std::string& message)
    {
        if (Instance().info_)
        {
            Instance().LogMessage("[Info] " + message + "\n");
        }
    }

    void Logger::LogError(const std::string& message)
    {
        Instance().LogMessage("[Error] " + message + "\n");
    }

    void Logger::LogDebug(const std::string& message)
    {
        if (Instance().debug_)
        {
            Instance().LogMessage("[Debug] " + message + "\n");
        }
    }

    void Logger::SetDebug(bool debug)
    {
        Instance().debug_ = debug;
    }

    void Logger::SetInfo(bool info)
    {
        Instance().info_ = info;
    }

    bool Logger::IsDebugEnabled()
    {
        return Instance().debug_;
    }

    bool Logger::IsInfoEnabled()
    {
        return Instance().info_;
    }
}

