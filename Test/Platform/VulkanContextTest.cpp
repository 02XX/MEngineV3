#include "VulkanContext.hpp"
#include <GLFW/glfw3.h>
#include <gtest/gtest.h>
#include <memory>
using namespace MEngine;
class VulkanContextTest : public ::testing::Test
{
  protected:
    GLFWwindow *window;
    void SetUp() override
    {
        if (!glfwInit())
        {
            throw std::runtime_error("Failed to initialize GLFW");
        }
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // No OpenGL context
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);   // Non-resizable window
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);     // Invisible window for Vulkan context creation
        window = glfwCreateWindow(640, 480, "Vulkan Context Test", nullptr, nullptr);
        if (!window)
        {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }
        glfwSetWindowUserPointer(window, this); // Set user pointer for potential use in Vulkan context
    }

    void TearDown() override
    {
        glfwTerminate();
    }
};

TEST_F(VulkanContextTest, Initialization)
{
    std::shared_ptr<VulkanContext> context = std::make_shared<VulkanContext>();
    EXPECT_NO_THROW(context->Init());
}
TEST_F(VulkanContextTest, SurfaceCreation)
{

    uint32_t vulkanInstanceExtensionCount = 0;
    const char **vulkanInstanceExtensions = glfwGetRequiredInstanceExtensions(&vulkanInstanceExtensionCount);
    std::vector<const char *> extensions(vulkanInstanceExtensions,
                                         vulkanInstanceExtensions + vulkanInstanceExtensionCount);
    std::shared_ptr<VulkanContextConfig> config = std::make_shared<VulkanContextConfig>();
    config->InstanceRequiredExtensions = extensions;
    config->InstanceRequiredLayers = {"VK_LAYER_KHRONOS_validation"};
    std::shared_ptr<VulkanContext> context = std::make_shared<VulkanContext>(config);
    VkSurfaceKHR surface;
    glfwCreateWindowSurface(context->GetInstance(), window, nullptr, &surface);
    context->InitSurface(surface);
    EXPECT_NO_THROW(context->Init());
}
