/**
* Created by luozhi on 9/9/23.
* author: luozhi
* maintainer: luozhi
*/
#ifndef REDIS_CPP_DRIVER_LOGGER_H
#define REDIS_CPP_DRIVER_LOGGER_H

#include <glog/logging.h>
#include <cstring>
#include <string>
#include <cstdarg>

class Logger {
public:
    explicit Logger(std::string&& appName) : appName_(std::move(appName)) {
        google::InitGoogleLogging(appName_.c_str(), &Logger::UserHeader);
        FLAGS_stderrthreshold = google::WARNING;
    }

    static void SetLogLevel(const int level) {
        FLAGS_stderrthreshold = level;
    }

    static std::string Format(const char* format, ...) {
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
    static void UserHeader(std::ostream& s, const google::LogMessageInfo& l, void* data) {
        char buf[LOG_ONE_LINE];
        snprintf(buf, LOG_ONE_LINE, "[%7s][%04d%02d%02d %02d:%02d:%02d.%06d][%u][%s:%d]",
                l.severity, l.time.year(), l.time.month(), l.time.day(), l.time.hour(), l.time.min(), l.time.sec(),
                l.time.usec(), l.thread_id, l.filename, l.line_number);
        s << buf;
    };

    std::string appName_; // keep argv0 valid
    static constexpr uint LOG_ONE_LINE = 1000U;
};

#define WARN(...) LOG(WARNING)<<Logger::Format(__VA_ARGS__)
#define ERROR(...) LOG(ERROR)<<Logger::Format(__VA_ARGS__)
#define INFO(...) LOG(INFO)<<Logger::Format(__VA_ARGS__)
#define FATAL(...) LOG(FATAL)<<Logger::Format(__VA_ARGS__)

static Logger logger("");

#endif // REDIS_CPP_DRIVER_LOGGER_H
