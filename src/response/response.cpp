/*
* Created by luozhi on 2023/09/16.
* author: luozhi
* maintainer: luozhi
*/

#include "response.h"

namespace RedisCpp {
Response::Response() {
    dataType_ = NIL;
}

const std::vector<std::string>& Response::Value() const {
    return data_;
}

void Response::SetDataType(const uint dataType) {
    dataType_ = dataType;
}

uint Response::GetDataType() const {
    return dataType_;
}

void Response::PushBack(std::string data) {
    data_.push_back(std::move(data));
}
} // RedisCpp