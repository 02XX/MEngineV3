#pragma once
#include "IMThread.hpp"
#include <concurrentqueue/concurrentqueue.h>
namespace MEngine::Core::Thread
{
class MRenderThread final : public IMThread
{
  private:
    moodycamel::ConcurrentQueue<std::function<void()>> mTaskQueue;
    std::jthread mThread;
    std::atomic<bool> mIsRunning{false};

  private:
    MRenderThread() = default;

  public:
    ~MRenderThread() override
    {
        Stop();
    };
    void Start() override;
    void Stop() override;
    bool IsRunning() const override;
    void AddTask(const std::function<void()> &task) override;
    // Singleton
    static MRenderThread &GetInstance();
};
} // namespace MEngine::Core::Thread