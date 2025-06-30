#pragma once
#include "ILogger.hpp"
#include <memory>

namespace MEngine
{

class Logger final
{
  private:
    Logger();
    std::shared_ptr<ILogger> mLogger = nullptr;

  public:
    static Logger &GetInstance();
    std::shared_ptr<ILogger> GetLogger() const
    {
        return mLogger;
    }
};

#define LogTrace(fmt, ...)                                                                                             \
    MEngine::Logger::GetInstance().GetLogger()->Trace(std::source_location::current(), fmt, ##__VA_ARGS__)
#define LogDebug(fmt, ...)                                                                                             \
    MEngine::Logger::GetInstance().GetLogger()->Debug(std::source_location::current(), fmt, ##__VA_ARGS__)
#define LogWarn(fmt, ...)                                                                                              \
    MEngine::Logger::GetInstance().GetLogger()->Warn(std::source_location::current(), fmt, ##__VA_ARGS__)
#define LogInfo(fmt, ...)                                                                                              \
    MEngine::Logger::GetInstance().GetLogger()->Info(std::source_location::current(), fmt, ##__VA_ARGS__)
#define LogError(fmt, ...)                                                                                             \
    MEngine::Logger::GetInstance().GetLogger()->Error(std::source_location::current(), fmt, ##__VA_ARGS__)
#define LogFatal(fmt, ...)                                                                                             \
    MEngine::Logger::GetInstance().GetLogger()->Fatal(std::source_location::current(), fmt, ##__VA_ARGS__)
} // namespace MEngine
