/*
* Created by luozhi on 2023/09/10.
* author: luozhi
* maintainer: luozhi
*/

#include "network.h"
#include "logger/logger.h"
#include <algorithm>
#include <netinet/tcp.h>

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

bool Network::Send(const std::string&& command) const {
    if (!alive_) {
        WARN("connection is inactive! please connect first.");
        return false;
    }
    INFO("%lu, %s", command.length(), command.c_str());

    auto len = send(socketHandle_, command.c_str(), command.length(), 0);
    if (len != command.length()) {
        WARN("command send failed, total %ul, send %ul.", command.length(), len);
        return false;
    }
    return true;
}

void Network::Close() {
    if (!alive_) {
        return;
    }
    shutdown(socketHandle_, SHUT_RDWR);
    alive_ = false;
}

bool Network::SetTimeout(const uint sec, const uint usec) {
    timeout_.tv_sec = sec;
    timeout_.tv_usec = usec;
    return setsockopt(socketHandle_, SOL_SOCKET, SO_RCVTIMEO,
                      reinterpret_cast<const char*>(&timeout_), sizeof(timeout_));
}

bool Network::Receive(std::string& reply) const {
    char buf[BUF_LENGTH];
    reply.clear();
    uint len = BUF_LENGTH - 1;
    while(len == BUF_LENGTH - 1) { // 如果一次buff接受不完数据，则继续接收
        len = recv(socketHandle_, buf, len, 0); // 留一位给字符串结束符
        if (len == -1) {
            WARN("receive failed or timeout!"); // 有可能上一次长度刚好和buffer一致，这时候下一次超时是正常情况
            break;
        }
        buf[len] = '\0';
        reply += buf;
        INFO("len: %d, %s", len, buf);
    }
    return reply.length();
}

bool Network::IsAlive() const {
    tcp_info info{};
    int len=sizeof(info);
    getsockopt(socketHandle_, IPPROTO_TCP, TCP_INFO, &info, (socklen_t *)&len);
    if(info.tcpi_state == TCP_ESTABLISHED) {
        return true;
    }
    return false;
}

} // RedisCpp