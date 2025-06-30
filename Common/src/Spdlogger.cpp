#include "Spdlogger.hpp"
#include "Configure.hpp"
#include "ILogger.hpp"
#include <iostream>
#include <magic_enum/magic_enum.hpp>
#include <string>


namespace MEngine
{
class UppercaseLevelFormatter : public spdlog::custom_flag_formatter
{
  public:
    void format(const spdlog::details::log_msg &msg, const std::tm &, spdlog::memory_buf_t &dest) override
    {
        std::string level = spdlog::level::to_string_view(msg.level).data();
        std::transform(level.begin(), level.end(), level.begin(), tolower);
        level[0] = toupper(level[0]);
        dest.append(level.data(), level.data() + level.size());
    }

    std::unique_ptr<custom_flag_formatter> clone() const override
    {
        return std::make_unique<UppercaseLevelFormatter>();
    }
};
Spdlogger::Spdlogger()
{
    try
    {
        // 读取配置
        mConfigure = std::make_shared<Configure>();
        auto json = mConfigure->GetJson();
        auto loggerConfig = json.get<LoggerConfig>();
        auto logFilePath = loggerConfig.logFilePath;
        auto logLevel = magic_enum::enum_cast<LogLevel>(loggerConfig.logLevel).value();
        std::cout << "Logger Level: " << magic_enum::enum_name(logLevel) << std::endl;
        std::cout << "Logger File Path: " << logFilePath << std::endl;

        mConsoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        mFileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(logFilePath, 5 * 1024 * 1024, 3);
        mLogger = std::make_shared<spdlog::logger>("MEngineLogger", spdlog::sinks_init_list{mConsoleSink, mFileSink});
        SetLevel(logLevel);
        auto formatter = std::make_unique<spdlog::pattern_formatter>();
        formatter->add_flag<UppercaseLevelFormatter>('U');
        formatter->set_pattern(loggerConfig.logFormat);
        mLogger->set_formatter(std::move(formatter));
    }
    catch (const spdlog::spdlog_ex &e)
    {
        std::cerr << "Logger initialization failed: " << e.what() << std::endl;
    }
}
void Spdlogger::SetLevel(LogLevel level)
{
    switch (level)
    {
    case LogLevel::Trace:
        mLogger->set_level(spdlog::level::trace);
        break;
    case LogLevel::Debug:
        mLogger->set_level(spdlog::level::debug);
        break;
    case LogLevel::Info:
        mLogger->set_level(spdlog::level::info);
        break;
    case LogLevel::Warn:
        mLogger->set_level(spdlog::level::warn);
        break;
    case LogLevel::Error:
        mLogger->set_level(spdlog::level::err);
        break;
    case LogLevel::Fatal:
        mLogger->set_level(spdlog::level::critical);
        break;
    }
}
void Spdlogger::Log(LogLevel level, const std::source_location &loc, const std::string &message)
{
    switch (level)
    {
    case LogLevel::Trace:
        mLogger->log(spdlog::source_loc{loc.file_name(), static_cast<int>(loc.line()), loc.function_name()},
                     spdlog::level::trace, message);
        break;
    case LogLevel::Debug:
        mLogger->log(spdlog::source_loc{loc.file_name(), static_cast<int>(loc.line()), loc.function_name()},
                     spdlog::level::debug, message);
        break;
    case LogLevel::Info:
        mLogger->log(spdlog::source_loc{loc.file_name(), static_cast<int>(loc.line()), loc.function_name()},
                     spdlog::level::info, message);
        break;
    case LogLevel::Warn:
        mLogger->log(spdlog::source_loc{loc.file_name(), static_cast<int>(loc.line()), loc.function_name()},
                     spdlog::level::warn, message);
        break;
    case LogLevel::Error:
        mLogger->log(spdlog::source_loc{loc.file_name(), static_cast<int>(loc.line()), loc.function_name()},
                     spdlog::level::err, message);
        break;
    case LogLevel::Fatal:
        mLogger->log(spdlog::source_loc{loc.file_name(), static_cast<int>(loc.line()), loc.function_name()},
                     spdlog::level::critical, message);
        break;
    }
}
} // namespace MEngine
