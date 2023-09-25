/*
* Created by luozhi on 2023/09/16.
* author: luozhi
* maintainer: luozhi
*/

#include "parser.h"
#include "logger/logger.h"

namespace RedisCpp {
std::unordered_map<uint, Parser::parserFunction> Parser::parser_;

uint Parser::Split(const std::string& str, RedisCpp::Response& result) {
    size_t end = str.find("\r\n");
    if (end == std::string::npos) { // msg broken
        ERROR("msg broken! cannot found end symbols '\\r\\n'");
        result.SetDataType(Response::NIL);
        return 0U;
    }
    result.PushBack(str.substr(0U, end));
    return end + 1U + 2U;
}

std::vector<RedisCpp::Response> Parser::parse(const std::string &&rsp) {
    data_.clear();
    uint pos = 0U;
    while(pos < rsp.length()) {
        RedisCpp::Response tmp;
        auto func = parser_[static_cast<uint>(rsp[pos])];
        uint count = 1U;
        if (func) { // 如果没有找到匹配的消息头，则跳过
            count += func(rsp.substr(pos + 1U, rsp.length() - pos), tmp);
        } else {
            ERROR("cannot found matched parser! please check parser of '%c', pos=%d", rsp[pos], pos);
        }
        pos += count;
        data_.push_back(std::move(tmp));
    }
    return data_;
}

bool Parser::RegisterParser(const uint flag, const parserFunction &parser) {
    if (parser_.count(flag) != 0U) {
        WARN("replace non-empty parser of \'%c\'!", flag);
    }
    parser_[flag] = parser;
    return false;
}

Parser::Parser() {
    RegisterParser(static_cast<uint>(':'), &Parser::IntegerParser);
    RegisterParser(static_cast<uint>('+'), &Parser::StatusParser);
    RegisterParser(static_cast<uint>('-'), &Parser::StatusParser);
    RegisterParser(static_cast<uint>('$'), &Parser::BulkStringsParser);
    RegisterParser(static_cast<uint>('*'), &Parser::ArrayParser);
}

uint Parser::IntegerParser(const std::string &reply, RedisCpp::Response& result) {
    result.SetDataType(Response::INT);
    return Split(reply, result);
}

uint Parser::StatusParser(const std::string &reply, RedisCpp::Response& result) {
    result.SetDataType(Response::STATUS);
    return Split(reply, result);
}

uint Parser::BulkStringsParser(const std::string &reply, Response &result) {
    result.SetDataType(Response::STRING);

    size_t end = reply.find("\r\n");
    if (end == std::string::npos) { // msg broken
        ERROR("msg broken! cannot found end symbols '\\r\\n'");
        result.SetDataType(Response::NIL);
        return 0U;
    }

    int len = std::stoi(reply.substr(0U, end));
    if (len < 0) { // 服务端返回了-1，这个key不存在
        result.SetDataType(Response::NIL);
        len = 0;
    } else {
        result.PushBack(reply.substr(end + 2U, len));
        len += 2; // ends with '\r\n'
    }
    return (end + 1U) + 2U + len;
}

uint Parser::ArrayParser(const std::string &reply, Response &result) {
    return 0;
}

std::string Parser::Decode(const std::string &msg) {
    std::string dst = msg;
    std::string::size_type pos = 0;
    while((pos = dst.find("\r\n")) != std::string::npos) {
        dst.replace(pos, 2U, "\\r\\n");
    }
    return std::move(dst);
}
} // RedisCpp