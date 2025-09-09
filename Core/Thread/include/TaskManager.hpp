#pragma once
#include <taskflow/taskflow.hpp>

namespace MEngine::Core::Thread
{
class TaskManager
{
  private:
    TaskManager() = default;

  public:
    TaskManager(const TaskManager &) = delete;
    TaskManager &operator=(const TaskManager &) = delete;
    TaskManager(TaskManager &&) = delete;
    TaskManager &operator=(TaskManager &&) = delete;
    ~TaskManager() = default;
    static tf::Executor &GetExecutor();
};

} // namespace MEngine::Core::Thread