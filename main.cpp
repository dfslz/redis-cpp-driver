#include <iostream>
#include <glog/logging.h>
#include "logger/logger.h"

int main(int argc, char* argv[]) {
    Logger::SetLogLevel(google::INFO);
    WARN("test logger warn");
    ERROR("test error parser %d\0", 5);
    INFO("test info %s", "test param");
    FATAL("test fatal");

    return 0;
}