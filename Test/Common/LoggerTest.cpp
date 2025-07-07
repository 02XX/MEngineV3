#include "Logger.hpp"
#include "gtest/gtest.h"
#include <gtest/gtest.h>
#include <string>
using namespace MEngine;
TEST(LoggerTest, singleton)
{
    Logger &logger = Logger::GetInstance();
    EXPECT_NE(logger.GetLogger(), nullptr);
}
TEST(LoggerTest, log)
{
    LogTrace("Test Trace, {}, {}, {}, {:.1f}", 1, "test", true, 3.14);
    LogDebug("Test Debug, {}, {}, {}, {:.1f}", 1, "test", true, 3.14);
    LogInfo("Test Info, {}, {}, {}, {:.1f}", 1, "test", true, 3.14);
    LogWarn("Test Warn, {}, {}, {}, {:.1f}", 1, "test", true, 3.14);
    LogError("Test Error, {}, {}, {}, {:.1f}", 1, "test", true, 3.14);
    LogFatal("Test Fatal, {}, {}, {}, {:.1f}", 1, "test", true, 3.14);
}
