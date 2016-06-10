//
// Created by wrede on 12.05.16.
//

#ifndef GBMOT_LOGGER_H
#define GBMOT_LOGGER_H
#include <string>

namespace util
{
    /**
     * Utility class for logging.
     * Is a singleton.
     * Provides three different types of messages.
     */
    class Logger
    {
    private:
        /**
         * -> Singleton
         */
        Logger();

        /**
         * True, if the info messages should be logged
         */
        bool info_;

        /**
         * True, if the debug messages should be logged
         */
        bool debug_;

        /**
         * Logs the given message.
         * @param message The message to log
         */
        void LogMessage(const std::string& message);
    public:
        /**
         * -> Singleton
         */
        Logger(Logger const&) = delete;

        /**
         * -> Singleton
         */
        void operator=(Logger const&) = delete;

        /**
         * Gets THE instance of this singleton.
         * Creates a new instance if not already created.
         */
        static Logger& Instance()
        {
            static Logger instance;
            return instance;
        }

        /**
         * Sets the debug message logging.
         * @param debug True, if the debug messages should be logged
         */
        static void SetDebug(bool debug);

        /**
         * Sets the info message logging.
         * @param info True, if the info messages should be logged
         */
        static void SetInfo(bool info);

        /**
         * If the debug messages are logged.
         * @return True, if the debug messages are logged
         */
        static bool IsDebugEnabled();

        /**
         * If the info messages are logged.
         * @return True, if the info messages are logged
         */
        static bool IsInfoEnabled();

        /**
         * Logs the given message as an info message.
         * @param message The info message to log
         */
        static void LogInfo(const std::string& message);

        /**
         * Logs the given message as an error message.
         * @param message The error message to log
         */
        static void LogError(const std::string& message);

        /**
         * Logs the given message as an debug message.
         * @param message The debug message to log
         */
        static void LogDebug(const std::string& message);
    };
}


#endif //GBMOT_LOGGER_H
