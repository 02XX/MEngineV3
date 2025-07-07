
#include "MRenderThread.hpp"
#include <gtest/gtest.h>
using namespace MEngine;
TEST(RenderThreadTest, StartStop)
{
    MRenderThread &renderThread = MRenderThread::GetInstance();
    renderThread.Start();
    EXPECT_TRUE(renderThread.IsRunning());
    renderThread.Stop();
}

TEST(RenderThreadTest, AddTask)
{
    MRenderThread &renderThread = MRenderThread::GetInstance();
    renderThread.Start();
    EXPECT_TRUE(renderThread.IsRunning());

    bool taskExecuted = false;
    renderThread.AddTask([&taskExecuted]() {
        GTEST_LOG_(INFO) << "Task executed";
        taskExecuted = true;
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Allow time for the task to execute
    EXPECT_TRUE(taskExecuted);

    renderThread.Stop();
    EXPECT_FALSE(renderThread.IsRunning());
}