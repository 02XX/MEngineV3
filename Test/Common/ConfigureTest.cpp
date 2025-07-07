#include "Configure.hpp"
#include "IConfigure.hpp"
#include "ILogger.hpp"
#include <gtest/gtest.h>
#include <memory>

using namespace MEngine;
TEST(ConfigureTest, Parse)
{
    std::shared_ptr<IConfigure> configure = std::make_shared<Configure>();
    EXPECT_NE(configure, nullptr);
    EXPECT_NE(configure->GetJson(), nullptr);
    auto json = configure->GetJson();
}