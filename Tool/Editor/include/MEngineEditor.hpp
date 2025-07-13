#pragma once
#include "AssetDatabase.hpp"
#include "MAsset.hpp"
#include "MFolder.hpp"
#include "MTexture.hpp"
#include <cstdint>
#include <entt/entity/fwd.hpp>
#include <entt/meta/resolve.hpp>
#include <taskflow/core/taskflow.hpp>
#include <unordered_map>
#include <vector>
#define GLFW_INCLUDE_VULKAN
#include "MRenderSystem.hpp"
#include "Reflect.hpp"
#include <GLFW/glfw3.h>
#include <entt/entt.hpp>
#include <format>
#include <imgui.h>
#include <memory>
#include <string>
#include <vulkan/vulkan.hpp>

#include <ImGuizmo.h>

using namespace MEngine::Core::Asset;
using namespace MEngine::Function::System;
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
    std::shared_ptr<MRenderSystem> mRenderSystem;

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
    std::vector<Resolution> mResolutions = {{100, 100},   {800, 600},   {1280, 720}, {1920, 1080},
                                            {2560, 1440}, {3840, 2160}, {5120, 2880}};
    Resolution mCurrentResolution = {1280, 720};

    // UI
    std::unordered_map<MAssetType, std::shared_ptr<MTexture>> mAssetIconTextures;
    std::unordered_map<MAssetType, VkDescriptorSet> mAssetIcons;
    std::shared_ptr<MAsset> mSelectedAsset = nullptr;
    std::shared_ptr<MAsset> mHoveredAsset = nullptr;
    entt::entity mSelectedEntity = entt::null; // 当前选中的实体
    entt::entity mHoveredEntity = entt::null;  // 当前悬停的实体
    ImGuizmo::OPERATION mGuizmoOperation = ImGuizmo::TRANSLATE;
    ImGuizmo::MODE mGuizmoMode = ImGuizmo::LOCAL;
    entt::entity mEditorCameraEntity = entt::null; // 编辑器相机实体
    enum class InspectorTab
    {
        None = 0,
        COMPONENTS = 1,
        ASSETS = 2
    };
    InspectorTab mSelectedTab = InspectorTab::COMPONENTS; // 默认 Components 选中
    int mColumns = 10;                                    // 列数
  private:
    tf::Taskflow mTaskflow;
    GLFWwindow *mWindow = nullptr;
    bool mIsFullscreen = false;
    WindowConfig mWindowConfig;
    bool mIsRunning = true;
    ImGuiID mDockSpaceID = 0;
    vk::UniqueRenderPass mUIRenderPass;
    std::shared_ptr<MFolder> mRootFolder{};
    std::shared_ptr<MFolder> mCurrentFolder{};
    std::filesystem::path mCurrentPath{};
    void InitWindow();
    void InitVulkan();
    void InitImGui();
    void InitDataBase();
    void InitEditorCamera();
    void InitSystem();
    void SetViewPort();
    void HandleSwapchainOutOfDate();
    void UI();
    void RenderToolbarPanel();
    void RenderViewportPanel();
    void RenderHierarchyPanel();
    void RenderInspectorPanel();
    void RenderAssetPanel();

    void SetGLFWCallBacks();
    uint32_t mFrameCount = 2;
    uint32_t mCurrentFrameIndex = 0;
    std::vector<VkDescriptorSet> mViewPortDescriptorSets{};
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
    bool ReflectObject(entt::meta_any &instance, entt::meta_type type, bool head = true, std::string headName = {});

  private:
};
} // namespace MEngine::Editor