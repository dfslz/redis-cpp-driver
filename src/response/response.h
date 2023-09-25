/*
* Created by luozhi on 2023/09/16.
* author: luozhi
* maintainer: luozhi
*/

#ifndef REDIS_CPP_DRIVER_RESPONSE_H
#define REDIS_CPP_DRIVER_RESPONSE_H

#include <string>
#include <cstdint>
#include <cstdlib>
#include <map>
#include <typeinfo>
#include <any>
#include <vector>

namespace RedisCpp {

class Response {
public:
    Response();
    ~Response() = default;

    const std::vector<std::string>& Value() const;

    void SetDataType(const uint dataType);

    uint GetDataType() const;

    void PushBack(std::string data);

    enum : uint {
        NIL = 0U,
        INT = 1U,
        STRING = 2U,
        FLOAT = 3U,
        ARRAY = 4U,
        STATUS = 5U
    };

private:
    std::vector<std::string> data_;
    uint dataType_;
    std::map<std::string, uint> checker_ = { // todo:添加类型识别和转换功能，利用typeid检查合法性
            { typeid(int).name(), INT },
            { typeid(std::string).name(), STRING },
            { typeid(float).name(), FLOAT }
    };
};

} // RedisCpp

#endif //REDIS_CPP_DRIVER_RESPONSE_H
