/*
* Created by luozhi on 2023/09/13.
* author: luozhi
* maintainer: luozhi
*/

#ifndef REDIS_CPP_DRIVER_NETWORK_TEST_H
#define REDIS_CPP_DRIVER_NETWORK_TEST_H

#include <gtest/gtest.h>
#include <net/network.h>

class NetworkTest : public testing::Test {
public:
    void SetUp() override {

    }

    void TearDown() override {

    }

    static void SetUpTestSuite() {

    }

    static void TearDownTestSuite() {

    }
};

TEST_F(NetworkTest, ConstructorTest) {
    EXPECT_NO_THROW(RedisCpp::Network("127.0.0.1", 6379));
}


#endif // REDIS_CPP_DRIVER_NETWORK_TEST_H
