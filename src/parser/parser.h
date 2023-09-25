/*
* Created by luozhi on 2023/09/16.
* author: luozhi
* maintainer: luozhi
*/

#ifndef REDIS_CPP_DRIVER_PARSER_H
#define REDIS_CPP_DRIVER_PARSER_H

#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include "response/response.h"

namespace RedisCpp {

class Parser {
public:
    Parser();
    ~Parser() = default;

    /**
     * @brief 解析服务器回应，返回统一的Response数据
     * @param rsp 服务器回应
     * @return 分割后带数据类型的数组
     */
    std::vector<RedisCpp::Response> parse(const std::string&& rsp);

    /**
     * @brief 解析函数的参数定义
     */
    using parserFunction = std::function<uint(const std::string&, RedisCpp::Response&)>;

    /**
     * @brief 注册解析函数，重复的注册会覆盖，需要返回读取了多少个字符，包括'\\r\\n'两个字符
     * @param flag 匹配开头字符，决定调用这个解析函数的首字符
     * @param parser 解析函数，格式参考parserFunction定义
     * @return 返回读取了多少字符，用于下一轮解析定位
     */
    static bool RegisterParser(const uint flag, const parserFunction& parser);

    /**
     * @brief 对字符串中的'\\r\\n'进行转义，用于调试输出server端返回的字符串
     * @param msg: server端消息
     * @return 转义后可以输出不可见字符'\\r\\n'的字符串
     */
    static std::string Decode(const std::string& msg);

private:
    static uint Split(const std::string& str, RedisCpp::Response& result);
    static uint IntegerParser(const std::string& reply, RedisCpp::Response& result);
    static uint StatusParser(const std::string& reply, RedisCpp::Response& result);
    static uint BulkStringsParser(const std::string& reply, RedisCpp::Response& result);
    static uint ArrayParser(const std::string& reply, RedisCpp::Response& result);

    std::vector<RedisCpp::Response> data_;
    static std::unordered_map<uint, parserFunction> parser_;
};

} // RedisCpp

#endif // REDIS_CPP_DRIVER_PARSER_H
