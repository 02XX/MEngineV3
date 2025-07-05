#include "MRenderThread.hpp"

namespace MEngine::Core::Thread
{

void MRenderThread::Start()
{
    if (mIsRunning)
        return;
    mIsRunning = true;
    mThread = std::jthread([this](std::stop_token st) {
        while (!st.stop_requested())
        {
            std::function<void()> task;
            if (mTaskQueue.try_dequeue(task))
            {
                task();
            }
        }
        mIsRunning = false;
    });
}
void MRenderThread::Stop()
{
    if (!mIsRunning)
        return;
    mIsRunning = false;
    mThread.request_stop();
    if (mThread.joinable())
    {
        mThread.join();
    }
}
bool MRenderThread::IsRunning() const
{
    return mIsRunning;
}
void MRenderThread::AddTask(const std::function<void()> &task)
{
    if (!mIsRunning)
        return;
    mTaskQueue.enqueue(task);
}
// Singleton
MRenderThread &MRenderThread::GetInstance()
{
    static MRenderThread instance;
    return instance;
}

} // namespace MEngine::Core::Thread