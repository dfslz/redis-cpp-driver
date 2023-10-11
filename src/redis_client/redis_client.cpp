/*
* Created by luozhi on 2023/09/13.
* author: luozhi
* maintainer: luozhi
*/

#include "redis_client.h"

namespace RedisCpp {
RedisClient::RedisClient(const std::string &&ip, const uint32_t port) {
    connection_ = std::make_unique<Network>(std::forward<const std::string&&>(ip), port);
}

bool RedisClient::Subscribe(const std::string &&channel,
                            std::function<void(const std::vector<RedisCpp::Response> &)> &callback) {
    if (!connection_ || (clientStatus_ != NORMAL)) {
        return false;
    }
    clientStatus_ = LISTENING;
    listen_ = true;
    pool_ = std::make_unique<ThreadPool>(1); // 拉起一个线程监听订阅消息，并在收到消息后调用回调函数

    pool_->commit([&, callback]() { // callback需要值捕获，否则离开Subscribe后异步线程会调用空指针
        // 由于连接监听有超时，所以这里要一直轮询，直到收到退出命令为止，注意默认超时时间是1s，可以调用connection_->SetTimeout设置
        while(listen_) {
            // todo:轮询receive并调用回调函数
        }
    });
    return true;
}

bool RedisClient::Unsubscribe() {
    if (clientStatus_ != LISTENING) {
        return false;
    }
    listen_ = false;
    pool_.reset();
    connection_->Send("UNSUBSCRIBE\r\n");

    std::string msg;
    bool success = connection_->Receive(msg); // todo: 这里判断返回值是否正确，如果接收失败置为DISCONNECT
    clientStatus_ = NORMAL;
    return true;
}

bool RedisClient::Send(const std::string &&cmd) {
    if (!connection_ || (clientStatus_ != NORMAL)) {
        return false;
    }
    clientStatus_ = WAITING;
    const bool status = connection_->Send(std::forward<const std::string&&>(cmd));
    if (!status) { // 发送失败，认为断开连接了，在获取回复的时候返回错误
        clientStatus_ = DISCONNECT;
    } else {
        clientStatus_ = WAITING; // 发送成功，等待接收回复
    }
    return status;
}

const std::vector<Response>& RedisClient::GetResponse() {
    response_.clear();
    if (!connection_ || (clientStatus_ != WAITING)) {
        return response_;
    }

    std::string msg;
    bool success = connection_->Receive(msg);
    if (!success) {
        clientStatus_ = DISCONNECT; // 由于上面已经判断过waiting状态，所以这里接收失败认为是断开连接了
        return response_;
    }

    clientStatus_ = NORMAL; // 接收完成，恢复正常状态
    response_ = parser_.parse(std::forward<const std::string>(msg));
    return response_;
}

bool RedisClient::Connect() {
    if (!connection_ || !connection_->Connect()) {
        return false;
    }
    clientStatus_ = NORMAL;
    return true;
}
} // RedisCpp