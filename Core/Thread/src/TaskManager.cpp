#include "TaskManager.hpp"
namespace MEngine::Core::Thread
{
tf::Executor &TaskManager::GetExecutor()
{
    static tf::Executor executor;
    return executor;
}
} // namespace MEngine::Core::Thread