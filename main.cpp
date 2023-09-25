#include <utility>

#include "logger/logger.h"
#include "net/network.h"
#include "thread_pool/thread_pool.h"
#include "parser/parser.h"
#include "redis_client/redis_client.h"

int main(int argc, char* argv[]) {
    RedisCpp::Logger::SetLogLevel(google::INFO);

    RedisCpp::RedisClient client("127.0.0.1", 6379);
    if (!client.Connect()) {
        ERROR("connect failed");
    }

    if (!client.Send("set test2 1233.456\r\n")) {
        ERROR("send error");
    }

    auto rsp = client.GetResponse();
    for (const auto& r : rsp) {
        if (r.GetDataType() == RedisCpp::Response::NIL) {
            WARN("server return NIL");
        }
        auto tmp = r.Value();
        for (const auto& s : tmp) {
            WARN("server return : %s", s.c_str());
        }
    }

    return 0;
}