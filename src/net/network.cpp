/*
* Created by luozhi on 2023/09/10.
* author: luozhi
* maintainer: luozhi
*/

#include "network.h"
#include "logger/logger.h"
#include <algorithm>

namespace RedisCpp {
Network::Network(const std::string&& ip, const uint32_t port, const sa_family_t protocol) {
    address_ = {
        .sin_family = protocol,
        .sin_port = htons(port),
        .sin_addr = {
            .s_addr = inet_addr(ip.c_str())
        }
    };
}

Network::~Network() {
    Close();
}

bool Network::Connect() {
    if (alive_) {
        return true;
    }

    socketHandle_ = socket(address_.sin_family, SOCK_STREAM, 0);
    if (socketHandle_ == -1) {
        ERROR("create socket failed! protocol %u", address_.sin_family);
        return false;
    }

    int ret = setsockopt(socketHandle_, SOL_SOCKET, SO_RCVTIMEO,
                         reinterpret_cast<const char*>(&timeout_), sizeof(timeout_));
    if (ret == -1) {
        ERROR("set socket timeout failed!");
        return false;
    }

    ret = connect(socketHandle_, reinterpret_cast<const sockaddr *>(&address_), sizeof(address_));
    if (ret == -1) {
        ERROR("connect to server failed!");
        return false;
    }

    return alive_ = true;
}

bool Network::Send(const std::string&& command, std::string& reply) const {
    if (!alive_) {
        WARN("connection is inactive! please connect first.");
        return false;
    }

    auto len = send(socketHandle_, command.c_str(), command.length(), 0);
    if (len != command.length()) {
        WARN("command send failed, total %ul, send %ul.", command.length(), len);
        return false;
    }

    char buf[BUF_LENGTH];
    reply.clear();
    len = BUF_LENGTH - 1;
    while(len == BUF_LENGTH - 1) { // 如果一次buff接受不完数据，则继续接收
        len = recv(socketHandle_, buf, len, 0); // 留一位给字符串结束符
        if (len == -1) {
            WARN("receive failed or timeout!"); // 有可能上一次长度刚好和buffer一致，这时候下一次超时是正常情况
            break;
        }
        buf[len] = '\0';
        reply += buf;
        INFO("len: %d", len);
    }
    return !reply.empty();
}

void Network::Close() {
    if (!alive_) {
        return;
    }
    shutdown(socketHandle_, SHUT_RDWR);
    alive_ = false;
}

} // RedisCpp