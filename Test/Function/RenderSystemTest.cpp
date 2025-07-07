
#include "Logger.hpp"
#include <memory>

#define GLFW_INCLUDE_VULKAN
#include "VulkanContext.hpp"
#include <GLFW/glfw3.h>
#include <gtest/gtest.h>

using namespace MEngine;
class RenderSystemTest : public ::testing::Test
{
  protected:
    GLFWwindow *mWindow{nullptr};
    std::shared_ptr<VulkanContext> mContext;
    void SetUp() override
    {
        InitWindow();
        InitVulkan();
    }
    void TearDown() override
    {
    }
    void InitWindow()
    {
        if (!glfwInit())
        {
            LogError("Failed to initialize GLFW");
            return;
        }
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        mWindow = glfwCreateWindow(800, 600, "Test Window", nullptr, nullptr);

        if (!mWindow)
        {
            LogError("Failed to create GLFW window");
            glfwTerminate();
            return;
        }
        glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
        glfwMakeContextCurrent(mWindow);
        LogDebug("GLFW window created successfully: {}x{}", 800, 600);
    }
    void InitVulkan()
    {
        mContext = std::make_shared<VulkanContext>();
        uint32_t vulkanInstanceExtensionCount = 0;
        const char **vulkanInstanceExtensions = glfwGetRequiredInstanceExtensions(&vulkanInstanceExtensionCount);
        std::vector<const char *> extensions(vulkanInstanceExtensions,
                                             vulkanInstanceExtensions + vulkanInstanceExtensionCount);
        mContext->InitContext({
            .InstanceRequiredExtensions = extensions,
            .InstanceRequiredLayers = {"VK_LAYER_KHRONOS_validation"},
            .DeviceRequiredExtensions = {"VK_KHR_swapchain"},
        });
        VkSurfaceKHR surface;
        glfwCreateWindowSurface(mContext->GetInstance(), mWindow, nullptr, &surface);
        mContext->InitSurface(surface);
        mContext->Init();
        LogDebug("Vulkan context initialized successfully");
    }
};

TEST_F(RenderSystemTest, Initialization)
{
    // command buffer
    vk::CommandBufferAllocateInfo commandBufferAllocateInfo;
    commandBufferAllocateInfo.setCommandPool(mContext->GetGraphicsCommandPool())
        .setLevel(vk::CommandBufferLevel::ePrimary)
        .setCommandBufferCount(2);
    auto commandBuffers = mContext->GetDevice().allocateCommandBuffersUnique(commandBufferAllocateInfo);
    EXPECT_EQ(commandBuffers.size(), 2);
    // Semaphore
    vk::SemaphoreCreateInfo semaphoreCreateInfo;
    auto semaphore0 = mContext->GetDevice().createSemaphoreUnique(semaphoreCreateInfo);
    auto semaphore1 = mContext->GetDevice().createSemaphoreUnique(semaphoreCreateInfo);
    EXPECT_TRUE(semaphore0);
    EXPECT_TRUE(semaphore1);

    commandBuffers[0]->begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
    commandBuffers[0]->end();
    vk::SubmitInfo submitInfo0;
    std::vector<vk::PipelineStageFlags> waitStage0 = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
    submitInfo0.setCommandBuffers(commandBuffers[0].get())
        .setSignalSemaphores({semaphore0.get()})
        .setWaitSemaphores(semaphore1.get())
        .setWaitDstStageMask(waitStage0);
    commandBuffers[1]->begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
    commandBuffers[1]->end();
    vk::SubmitInfo submitInfo1;
    std::vector<vk::PipelineStageFlags> waitStage1 = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
    submitInfo1.setCommandBuffers(commandBuffers[1].get())
        .setSignalSemaphores({semaphore1.get()})
        .setWaitSemaphores({semaphore0.get()})
        .setWaitDstStageMask(waitStage1);

    mContext->GetGraphicsQueue().submit(submitInfo0, nullptr);
    mContext->GetGraphicsQueue().submit(submitInfo1, nullptr);
    mContext->GetGraphicsQueue().waitIdle();
}