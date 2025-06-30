#pragma once
#include "IConfigure.hpp"
#include "ILogger.hpp"
#include <memory>
#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace MEngine
{
struct LoggerConfig
{
    std::string logFilePath = "logs/app.log";
    std::string logLevel = "Trace";
    std::string logFormat = "%^[%Y-%m-%d %H:%M:%S.%e] [%20s:%-4#] %U: %v%$";
};
class Spdlogger final : public ILogger
{
  private:
    std::shared_ptr<IConfigure> mConfigure = nullptr;

  private:
    void Log(LogLevel level, const std::source_location &loc, const std::string &message) override;
    spdlog::sink_ptr mConsoleSink;
    spdlog::sink_ptr mFileSink;
    std::shared_ptr<spdlog::logger> mLogger;

  public:
    Spdlogger();
    void SetLevel(LogLevel level) override;
};
} // namespace MEngine

namespace nlohmann
{
template <> struct adl_serializer<MEngine::LoggerConfig>
{

    static void to_json(json &j, const MEngine::LoggerConfig &config)
    {
        j["Logger"]["LogFilePath"] = config.logFilePath;
        j["Logger"]["LogLevel"] = config.logLevel;
        j["Logger"]["LogFormat"] = config.logFormat;
    }
    static void from_json(const json &j, MEngine::LoggerConfig &config)
    {
        config.logFilePath = j.at("Logger").at("LogFilePath").get<std::string>();
        config.logLevel = j.at("Logger").at("LogLevel").get<std::string>();
        config.logFormat = j.at("Logger").at("LogFormat").get<std::string>();
    }
};
} // namespace nlohmann