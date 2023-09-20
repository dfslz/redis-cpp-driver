#include <utility>

#include "logger/logger.h"
#include "net/network.h"
#include "thread_pool/thread_pool.h"
#include "parser/parser.h"

int main(int argc, char* argv[]) {
    RedisCpp::Logger::SetLogLevel(google::INFO);

    INFO("connect to redis-server");
    RedisCpp::Network connection("127.0.0.1", 6379);
    if (!connection.Connect()) {
        ERROR("Connect failed");
    }

    std::string reply;
    connection.Send("object encoding test2\n", reply);
    auto trans = RedisCpp::Parser::Decode(reply);
    INFO("%s", trans.c_str());

    return 0;
}