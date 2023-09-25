/*
* Created by luozhi on 2023/09/13.
* author: luozhi
* maintainer: luozhi
*/

#ifndef REDIS_CPP_DRIVER_REDIS_CLIENT_H
#define REDIS_CPP_DRIVER_REDIS_CLIENT_H

#include <string>
#include <cstdint>
#include <memory>
#include <functional>
#include "response/response.h"
#include "net/network.h"
#include "parser/parser.h"
#include "thread_pool/thread_pool.h"

namespace RedisCpp {

class RedisClient {
public:
    RedisClient() = default;
    RedisClient(const std::string&& ip, const uint32_t port);
    ~RedisClient() = default;

    /**
     * @brief 尝试连接redis数据库
     * @return 连接成功/失败
     */
    bool Connect();

    /**
     * @brief 注册一个订阅回调，会另起一个线程执行，调用后在Unsubscribe之前无法向server端发送命令
     * @param channel 订阅频道
     * @param callback 处理消息回调
     * @return 注册成功/失败
     */
    bool Subscribe(const std::string&& channel, std::function<void(const std::vector<Response>&)>& callback);

    /**
     * @brief 取消订阅状态，如果没有处于订阅状态则会返回失败
     * @return 取消成功/失败
     */
    bool Unsubscribe();

    /**
     * @brief 向server端发送一个/一组命令，会异步执行，server端返回值可以通过GetResponse()获取
     * @param cmd Redis命令
     * @return 发送成功/失败
     */
    bool Send(const std::string&& cmd);

    /**
     * @brief 获取上一条命令的返回值解析结果，如果网络错误则返回空数组\n
     * @brief (btw. 如果网络没有异常，则至少有一条OK状态消息返回)
     * @return 解析server端返回值的结果数组
     */
    const std::vector<Response>& GetResponse();

private:
    enum : int {
        DISCONNECT = -1,
        NORMAL = 0,
        LISTENING = 1,
        WAITING = 2
    };

    std::unique_ptr<Network> connection_;
    std::unique_ptr<ThreadPool> pool_;
    std::vector<Response> response_;
    std::future<std::string> msg_;
    Parser parser_;
    std::function<void(const std::vector<Response>&)> callback_ = nullptr;
    int clientStatus_ = DISCONNECT;
};

} // RedisCpp

#endif // REDIS_CPP_DRIVER_REDIS_CLIENT_H
