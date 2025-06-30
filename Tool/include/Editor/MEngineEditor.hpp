#pragma once

#include "MTexture.hpp"
#include <cstdint>
#include <vector>
#include <vulkan/vulkan_handles.hpp>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <format>
#include <imgui.h>
#include <memory>
#include <string>
#include <vulkan/vulkan.hpp>
namespace MEngine::Editor
{
struct WindowConfig
{
    int32_t width = 1280;
    int32_t height = 720;
    std::string title = "MEngine Editor";
    bool fullscreen = false;
    bool resizable = true;
    bool vsync = true;
    std::string fontPath = "Assets/Fonts/NotoSans-Medium.ttf";
    float fontSize = 16.0f;
};
struct Resolution
{
    int width = 1280;
    int height = 720;
    std::string ToString()
    {
        std::string resolution = std::format("{}x{}", width, height);
        return resolution;
    }
    bool operator==(const Resolution &other) const
    {
        return width == other.width && height == other.height;
    }
};
class MEngineEditor
{
  public:
    MEngineEditor() = default;
    ~MEngineEditor() = default;

    // Initialize the editor
    void Init();

    // Run the editor loop
    void Run(float deltaTime);

    // Shutdown the editor
    void Shutdown();

  private:
    GLFWwindow *mWindow = nullptr;
    bool mIsFullscreen = false;
    WindowConfig mWindowConfig;
    bool mIsRunning = true;
    ImGuiID mDockSpaceID = 0;
    vk::UniqueRenderPass mUIRenderPass;
    void InitWindow();
    void InitVulkan();
    void InitImGui();
    void UI();
    void RenderToolbarPanel();
    void RenderViewportPanel();
    void RenderHierarchyPanel();
    void RenderInspectorPanel();
    void RenderAssetPanel();

    uint32_t mFrameCount = 2;
    uint32_t mCurrentFrameIndex = 0;
    std::vector<vk::UniqueFramebuffer> mUIFramebuffers;
    std::vector<vk::UniqueCommandBuffer> mUICmdBuffers;
    std::vector<vk::UniqueFence> mInFlightFences;
    std::vector<vk::UniqueSemaphore> mImageAvailableSemaphores;
    std::vector<vk::UniqueSemaphore> mRenderFinishedSemaphores;
    void CreateCommandBuffers();
    void CreateUIRenderPass();
    void CreateFramebuffer();
    void CreateFences();
    void CreateSemaphores();
};
} // namespace MEngine::Editor