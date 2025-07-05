#pragma once
#include "AssetDatabase.hpp"
#include "MAsset.hpp"
#include "MFolder.hpp"
#include "MTexture.hpp"
#include <cstdint>
#include <entt/meta/resolve.hpp>
#include <unordered_map>
#include <vector>
#define GLFW_INCLUDE_VULKAN
#include "Reflect.hpp"
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
  private:
    std::shared_ptr<AssetDatabase> mAssetDatabase;

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
    std::filesystem::path mProjectPath = "Project";

    // UI
    std::unordered_map<MAssetType, std::shared_ptr<MTexture>> mAssetIconTextures;
    std::unordered_map<MAssetType, VkDescriptorSet> mAssetIcons;
    std::shared_ptr<MAsset> mSelectedAsset = nullptr;
    std::shared_ptr<MAsset> mHoveredAsset = nullptr;
    enum class InspectorTab
    {
        None = 0,
        COMPONENTS = 1,
        ASSETS = 2
    };
    InspectorTab mSelectedTab = InspectorTab::COMPONENTS; // 默认 Components 选中
    int mColumns = 10;                                    // 列数
  private:
    GLFWwindow *mWindow = nullptr;
    bool mIsFullscreen = false;
    WindowConfig mWindowConfig;
    bool mIsRunning = true;
    ImGuiID mDockSpaceID = 0;
    vk::UniqueRenderPass mUIRenderPass;
    std::shared_ptr<MFolder> mRootFolder{};
    std::shared_ptr<MFolder> mCurrentFolder{};
    void InitWindow();
    void InitVulkan();
    void InitImGui();
    void InitDataBase();
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
    void ReflectObject(entt::meta_any &instance, entt::meta_type type);

  private:
    // UI控件
};
} // namespace MEngine::Editor