/*
* Created by luozhi on 2023/09/10.
* author: luozhi
* maintainer: luozhi
*/

#ifndef REDIS_CPP_DRIVER_NETWORK_H
#define REDIS_CPP_DRIVER_NETWORK_H

#include <string>
#include <cstdint>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace RedisCpp {

class Network {
public:
    Network(const std::string&& ip, const uint32_t port, const sa_family_t protocol = AF_INET);
    ~Network();

    /**
     * @brief 默认接收超时时间是1s,这么做是为了防止消息长度和buffer刚好相同时无法判断接收是否结束，需要等待异常状态来告诉
     * @brief 我们消息刚好结束了，这种情况应该很罕见，但是不排除出现，这时候可能存在1s的性能问题
     * @return 连接成功/失败
     */
    bool Connect();

    /**
     * @brief 关闭连接
     */
    void Close();

    /**
     * @brief 发送一条消息到服务端，并等待回复，默认1s没有消息则超时
     * @param message 往服务端发送的消息
     * @param reply 收到的回复
     * @return 发送成功且收到回复成功 todo: 发送和收回复应该分开，并且分别返回是否成功
     */
    bool Send(const std::string&& message) const;

    /**
     * 获取server端的消息，如果没有回复则等待超时返回空字符串，并返回false
     * @param reply
     * @return 接收成功/失败
     */
    bool Receive(std::string& reply) const;

    /**
     * 设置发送和接收消息的超时时间，默认不设置则为1s超时
     * @param sec 秒
     * @param usec 毫秒
     * @return 设置成功/失败
     */
    bool SetTimeout(const uint sec, const uint usec);

private:
    bool alive_{false};
    int socketHandle_{-1};
    sockaddr_in address_{};
    timeval timeout_{1, 0};

    static constexpr uint BUF_LENGTH = 1024 * 100; // 默认缓冲区100k，todo:改成可以设置的参数
};

} // RedisCpp

#endif //REDIS_CPP_DRIVER_NETWORK_H
