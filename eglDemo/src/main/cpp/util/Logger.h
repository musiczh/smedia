//
// Created by wansu on 2021/11/5.
//

#ifndef SMEDIA_LOGGER_H
#define SMEDIA_LOGGER_H
#include <sstream>
#include <iostream>
#include <thread>
#include <functional>
#include <sys/time.h>

enum LoggerLevel {
    ERROR,WARN,INFO,DEBUG
};
std::string getCurrentTime();

class Logger {
public:
    Logger();
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





#endif //SMEDIA_LOGGER_H
