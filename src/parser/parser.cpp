/*
* Created by luozhi on 2023/09/16.
* author: luozhi
* maintainer: luozhi
*/

#include "parser.h"
#include "logger/logger.h"

namespace RedisCpp {
std::unordered_map<uint, Parser::parserFunction> Parser::parser_;
std::vector<std::string> Parser::Split(std::string str, const std::string&& delimiter) {
    std::vector<std::string> result;

    while(!str.empty()) {
        auto pos = str.find(delimiter);
        if (pos != std::string::npos) {
            result.emplace_back(str.substr(0, pos));
            str = str.substr(pos, str.length() - pos);
        } else {
            result.emplace_back(str);
            break;
        }
    }
    return std::move(result);
}

std::vector<RedisCpp::Response> Parser::parse(const std::string &&rsp) {
    data_.clear();
    uint pos = 0U;
    while(pos < rsp.length()) {
        RedisCpp::Response tmp;
        auto func = parser_[static_cast<uint>(rsp[pos])];
        uint count = func(rsp.substr(pos, rsp.length() - pos), tmp);
        pos += count;
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
    RegisterParser(static_cast<uint>('+'), &Parser::SimpleStringParser);
    RegisterParser(static_cast<uint>('-'), &Parser::StatusParser);
    RegisterParser(static_cast<uint>('$'), &Parser::BulkStringsParser);
    RegisterParser(static_cast<uint>('*'), &Parser::ArrayParser);
}

uint Parser::IntegerParser(const std::string &reply, RedisCpp::Response& result) {
    uint count = 1U;
    size_t end = reply.find("\r\n", count);
    if (end == std::string::npos) { // msg broken
        ERROR("msg broken! cannot found end symbols '\\r\\n'");
        return count;
    }

    result.SetDataType(Response::INT);
    return count;
}

uint Parser::StatusParser(const std::string &reply, RedisCpp::Response& result) {
    return {};
}

uint Parser::SimpleStringParser(const std::string &reply, Response &result) {
    return 0;
}

uint Parser::BulkStringsParser(const std::string &reply, Response &result) {
    return 0;
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