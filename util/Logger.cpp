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

    void Logger::LogInfo(const std::string& message)
    {
        if (Instance().info_)
        {
            std::cout << "[Info] " << message << std::endl;
        }
    }

    void Logger::LogError(const std::string& message)
    {
        std::cout << "[Error] " << message << std::endl;
    }

    void Logger::LogDebug(const std::string& message)
    {
        if (Instance().debug_)
        {
            std::cout << "[Debug] " << message << std::endl;
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

