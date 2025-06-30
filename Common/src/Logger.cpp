#include "Logger.hpp"
#include "Spdlogger.hpp"
namespace MEngine
{
Logger &Logger::GetInstance()
{
    static Logger instance;
    return instance;
}
Logger::Logger()
{
    mLogger = std::make_shared<Spdlogger>();
}
} // namespace MEngine