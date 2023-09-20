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
    void Close();
    bool Send(const std::string&& command, std::string& reply) const;

private:
    bool alive_{false};
    int socketHandle_{-1};
    sockaddr_in address_{};
    const timeval timeout_{1, 0};

    static constexpr uint BUF_LENGTH = 1024 * 100; // default 100k
};

} // RedisCpp

#endif //REDIS_CPP_DRIVER_NETWORK_H
