/*
* Created by luozhi on 2023/09/09.
* author: luozhi
* maintainer: luozhi
*/
#ifndef REDIS_CPP_DRIVER_LOGGER_H
#define REDIS_CPP_DRIVER_LOGGER_H

#include <glog/logging.h>
#include <cstring>
#include <string>
#include <cstdarg>

namespace RedisCpp {
class Logger {
public:
    explicit Logger(std::string &&appName) : appName_(std::move(appName)) {
        google::InitGoogleLogging(appName_.c_str(), &Logger::UserHeader);
        FLAGS_stderrthreshold = google::WARNING;
    }

    static void SetLogLevel(const int level) {
        FLAGS_stderrthreshold = level;
    }

    static std::string Format(const char *format, ...) {
        va_list list;
        va_start(list, format);
        char buf[LOG_ONE_LINE];
        vsnprintf(buf, LOG_ONE_LINE, format, list);
        va_end(list);
        return {buf};
    }

    ~Logger() {
        google::ShutdownGoogleLogging();
    }

private:
    static void UserHeader(std::ostream &s, const google::LogMessageInfo &l, void *data) {
        char buf[LOG_ONE_LINE];
        snprintf(buf, LOG_ONE_LINE, "[%5s][%04d%02d%02d %02d:%02d:%02d.%06d][%u][%s:%d]",
                 mapping + (strchr(strSearch, l.severity[0]) - strSearch),
                 l.time.year(), l.time.month(), l.time.day(), l.time.hour(), l.time.min(), l.time.sec(),
                 l.time.usec(), l.thread_id, l.filename, l.line_number);
        s << buf;
    };

    std::string appName_; // keep argv0 valid
    static constexpr uint LOG_ONE_LINE = 2048U;
    static constexpr const char* mapping = "FATAL\0WARN\0ERROR\0INFO";
    static constexpr const char* strSearch = "FATAL WARN ERROR INFO";
};
}

#define WARN(...) LOG(WARNING) << RedisCpp::Logger::Format(__VA_ARGS__)
#define ERROR(...) LOG(ERROR) << RedisCpp::Logger::Format(__VA_ARGS__)
#define INFO(...) LOG(INFO) << RedisCpp::Logger::Format(__VA_ARGS__)
#define FATAL(...) LOG(FATAL) << RedisCpp::Logger::Format(__VA_ARGS__)

#endif // REDIS_CPP_DRIVER_LOGGER_H
