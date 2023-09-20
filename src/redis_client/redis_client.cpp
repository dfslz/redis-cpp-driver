/*
* Created by luozhi on 2023/09/13.
* author: luozhi
* maintainer: luozhi
*/

#include "redis_client.h"

namespace RedisCpp {
RedisClient::RedisClient(const std::string &&ip, const uint32_t port) {
    connection_ = std::make_unique<Network>(std::forward<const std::string&&>(ip), port);
    pool_ = std::make_unique<ThreadPool>(1);
}

bool RedisClient::Subscribe(const std::string &&channel,
                            std::function<void(const std::vector<RedisCpp::Response> &)> &callback) {
    return false;
}

bool RedisClient::Unsubscribe() {
    return false;
}

bool RedisClient::Execute(const std::string &&cmd) {
    if (!connection_ || (clientStatus_ != NORMAL)) {
        return false;
    }
    clientStatus_ = WAITING;
    *msg_ = std::move(pool_->commit([&]() {
        std::string reply;
        const bool status = connection_->Send(std::forward<const std::string&&>(cmd), reply);
        if (!status) { // 发送失败，认为网络断了，在获取回复的时候返回错误
            clientStatus_ = DISCONNECT;
        }
        clientStatus_ = NORMAL;
        return reply;
    }));
    return true;
}

const std::vector<Response> &RedisClient::GetResponse() {
    if (!connection_ || !msg_->valid()) {
        response_.clear();
        return response_;
    }
    std::string msg = msg_->get();
    msg_.reset(); // 释放这条消息，防止下一条命令误读

    response_ = parser_.parse(std::forward<const std::string>(msg));
    return response_;
}

bool RedisClient::Connect() {
    if (!connection_) {
        return false;
    }
    return connection_->Connect();
}
} // RedisCpp