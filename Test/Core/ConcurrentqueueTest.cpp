#include "gtest/gtest.h"
#include <chrono>
#include <concurrentqueue/concurrentqueue.h>
#include <gtest/gtest.h>
#include <thread>
TEST(ConcurrentQueueTest, BasicOperations)
{
    moodycamel::ConcurrentQueue<int> queue;
    // producer
    std::jthread producer([&queue]() {
        GTEST_LOG_(INFO) << "Starting producer thread";
        for (int i = 0; i < 100; ++i)
        {
            queue.enqueue(i);
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    });
    // consumer
    std::jthread consumer([&queue]() {
        int value;
        GTEST_LOG_(INFO) << "Starting consumer thread";
        while (true)
        {
            if (queue.try_dequeue(value))
            {
                GTEST_LOG_(INFO) << "Dequeued value: " << value;
            }
            else
            {
                GTEST_LOG_(INFO) << "Queue is empty, waiting for items";
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });
}