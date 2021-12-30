//
// Created by wansu on 2021/11/5.
//

#include "Logger.h"
namespace smedia{
    const char* tag = "huan [native]";

    std::stringstream& Logger::Get(const char *fileName, LoggerLevel level) {
        mLogLevel = level;
        os << getCurrentTime() << "\t";
        os << "[" << std::this_thread::get_id() << "]\t";
        os << "[" << fileName << "]\t" ;
        return os;
    }

    Logger::~Logger() {
        if (logFunction != nullptr) {
            logFunction(mLogLevel,os.str().c_str());
        } else {
            std::cout << os.str() << std::endl;
        }
    }

    void Logger::setLogFunction(void (*p)(LoggerLevel, const char *)) {
        logFunction = p;
    }

    std::string getCurrentTime(){
        char buffer[22];
        time_t t;
        time(&t);
        tm r = {0};
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %X", localtime_r(&t, &r));
        struct timeval tv{};
        gettimeofday(&tv, nullptr);
        char result[100] = {0};
        std::sprintf(result, "%s.%03ld", buffer, (long)tv.tv_usec / 1000);
        return result;
    }

    std::function<void(LoggerLevel, const char *)> Logger::logFunction;


}

