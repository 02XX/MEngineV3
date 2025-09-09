#pragma once
#include <functional>
namespace MEngine::Core::Thread
{
class IMThread
{
  public:
    virtual ~IMThread() = default;

    virtual void Start() = 0;
    virtual void Stop() = 0;
    virtual bool IsRunning() const = 0;
    virtual void AddTask(const std::function<void()> &task) = 0;
};
} // namespace MEngine::Core::Thread