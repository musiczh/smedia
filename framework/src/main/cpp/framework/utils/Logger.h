//
// Created by wansu on 2021/11/5.
//

#ifndef SMEDIA_LOGGER_H
#define SMEDIA_LOGGER_H
#include <iostream>
#include <functional>
#include <sstream>
#include <sys/time.h>
#include <thread>
#include <android/log.h>
namespace smedia {
    enum LoggerLevel {
        ERROR,WARN,INFO,DEBUG
    };
    std::string getCurrentTime();

    class Logger {
    public:
        Logger() = default;
        std::stringstream& Get(const char* fileName,LoggerLevel level);
        static void setLogFunction(void (*p)(LoggerLevel,const char*));
        ~Logger();
    private:
        static std::function<void(LoggerLevel, const char *)> logFunction;
        LoggerLevel mLogLevel;
        std::stringstream os;
    };

    #define LOG_INFO Logger().Get(__FILE_NAME__,LoggerLevel::INFO)
    #define LOG_WARN Logger().Get(__FILE_NAME__,LoggerLevel::WARN)
    #define LOG_ERROR Logger().Get(__FILE_NAME__,LoggerLevel::ERROR)
    #define LOG_DEBUG Logger().Get(__FILE_NAME__,LoggerLevel::DEBUG)
}




#endif //SMEDIA_LOGGER_H
