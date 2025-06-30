#include "Editor/MEngineEditor.hpp"
#include "Configure.hpp"
#include "Logger.hpp"
#include "VulkanContext.hpp"
#include <boost/di.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <imgui_internal.h>

namespace MEngine::Editor
{

namespace DI = boost::di;
auto injector = DI::make_injector(DI::bind<IConfigure>().to<Configure>().in(DI::unique),
                                  DI::bind<VulkanContext>().to<VulkanContext>().in(DI::singleton));
void MEngineEditor::Init()
{
    InitWindow();
    InitVulkan();
    InitImGui();
    mIsRunning = true;
    LogInfo("MEngine Editor initialized successfully");
}
void MEngineEditor::InitWindow()
{
    // 读取配置文件
    // mWindowConfig = injector.create<std::shared_ptr<IConfigure>>()->GetJson().get<WindowConfig>();
    mWindowConfig = {
        .width = 1280,
        .height = 720,
        .title = "MEngine Editor",
        .fullscreen = false,
    };
    if (!glfwInit())
    {
        LogError("Failed to initialize GLFW");
        return;
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, mWindowConfig.resizable ? GLFW_TRUE : GLFW_FALSE);
    if (mWindowConfig.fullscreen)
    {
        const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        mWindowConfig.width = mode->width;
        mWindowConfig.height = mode->height;
        mWindow = glfwCreateWindow(mWindowConfig.width, mWindowConfig.height, mWindowConfig.title.c_str(),
                                   glfwGetPrimaryMonitor(), nullptr);
        glfwWindowHint(GLFW_DECORATED, mWindowConfig.fullscreen ? GLFW_FALSE : GLFW_TRUE);
        mIsFullscreen = true;
    }
    else
    {
        mWindow =
            glfwCreateWindow(mWindowConfig.width, mWindowConfig.height, mWindowConfig.title.c_str(), nullptr, nullptr);
    }
    if (!mWindow)
    {
        LogError("Failed to create GLFW window");
        glfwTerminate();
        return;
    }
    if (mWindowConfig.vsync)
    {
        glfwSwapInterval(1); // 启用垂直同步
        LogInfo("VSync enabled");
    }
    else
    {
        glfwSwapInterval(0); // 禁用垂直同步
    }
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    glfwMakeContextCurrent(mWindow);
    LogDebug("GLFW window created successfully: {}x{}", mWindowConfig.width, mWindowConfig.height);
}
void MEngineEditor::InitVulkan()
{
    auto context = injector.create<std::shared_ptr<VulkanContext>>();
    uint32_t vulkanInstanceExtensionCount = 0;
    const char **vulkanInstanceExtensions = glfwGetRequiredInstanceExtensions(&vulkanInstanceExtensionCount);
    std::vector<const char *> extensions(vulkanInstanceExtensions,
                                         vulkanInstanceExtensions + vulkanInstanceExtensionCount);
    context->InitContext({
        .InstanceRequiredExtensions = extensions,
        .InstanceRequiredLayers = {"VK_LAYER_KHRONOS_validation"},
        .DeviceRequiredExtensions = {"VK_KHR_swapchain"},
    });
    VkSurfaceKHR surface;
    glfwCreateWindowSurface(context->GetInstance(), mWindow, nullptr, &surface);
    context->InitSurface(surface);
    context->Init();
    mFrameCount = context->GetSurfaceInfo().imageCount;
    LogDebug("Vulkan context initialized successfully");
}
void MEngineEditor::Run(float deltaTime)
{
    auto vulkanContext = injector.create<std::shared_ptr<VulkanContext>>();
    while (!glfwWindowShouldClose(mWindow))
    {
        glfwPollEvents();
        auto result = vulkanContext->GetDevice().waitForFences({mInFlightFences[mCurrentFrameIndex].get()}, vk::True,
                                                               1000000000); // 1s
        if (result != vk::Result::eSuccess)
        {
            LogError("Failed to wait for fence: {}", vk::to_string(result));
            continue;
        }
        vulkanContext->GetDevice().resetFences({mInFlightFences[mCurrentFrameIndex].get()});

        auto resultValue = vulkanContext->GetDevice().acquireNextImageKHR(
            vulkanContext->GetSwapchain(), 1000000000, mImageAvailableSemaphores[mCurrentFrameIndex].get(), nullptr);
        if (resultValue.result == vk::Result::eErrorOutOfDateKHR)
        {
            // HandleSwapchainOutOfDate();
        }

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        UI();
        if (mIsRunning)
        {
        }
        ImGui::Render();
        mUICmdBuffers[mCurrentFrameIndex]->reset();
        mUICmdBuffers[mCurrentFrameIndex]->begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
        vk::RenderPassBeginInfo renderPassInfo{};
        renderPassInfo.setRenderPass(mUIRenderPass.get())
            .setFramebuffer(mUIFramebuffers[mCurrentFrameIndex].get())
            .setRenderArea(
                {{0, 0}, {static_cast<uint32_t>(mWindowConfig.width), static_cast<uint32_t>(mWindowConfig.height)}})
            .setClearValues(
                {vk::ClearValue().setColor(vk::ClearColorValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}))});
        mUICmdBuffers[mCurrentFrameIndex]->beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), mUICmdBuffers[mCurrentFrameIndex].get());
        mUICmdBuffers[mCurrentFrameIndex]->endRenderPass();
        mUICmdBuffers[mCurrentFrameIndex]->end();
        vk::SubmitInfo submitInfo;
        vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        submitInfo.setCommandBuffers(mUICmdBuffers[mCurrentFrameIndex].get())
            .setSignalSemaphores(mRenderFinishedSemaphores[mCurrentFrameIndex].get())
            .setWaitSemaphores(mImageAvailableSemaphores[mCurrentFrameIndex].get())
            .setWaitDstStageMask({waitStage});

        vulkanContext->GetGraphicsQueue().submit(submitInfo, mInFlightFences[mCurrentFrameIndex].get());

        vk::PresentInfoKHR presentInfo;
        presentInfo.setSwapchains(vulkanContext->GetSwapchain())
            .setImageIndices({resultValue.value})
            .setWaitSemaphores(mRenderFinishedSemaphores[mCurrentFrameIndex].get());
        auto presentResult = vulkanContext->GetPresentQueue().presentKHR(presentInfo);
        if (presentResult == vk::Result::eErrorOutOfDateKHR || presentResult == vk::Result::eSuboptimalKHR)
        {
            // HandleSwapchainOutOfDate();
        }
        else if (presentResult != vk::Result::eSuccess)
        {
            LogError("Failed to present swapchain image: {}", vk::to_string(presentResult));
        }
        mCurrentFrameIndex = (mCurrentFrameIndex + 1) % mFrameCount;
    }
}
void MEngineEditor::InitImGui()
{
    CreateUIRenderPass();
    CreateFramebuffer();
    CreateCommandBuffers();
    CreateFences();
    CreateSemaphores();
    // ImGui初始化代码
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // 启用键盘导航
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // 启用Docking
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // 启用多视口支持
    // 设置字体
    // io.Fonts->AddFontFromFileTTF(mWindowConfig.fontPath.c_str(), mWindowConfig.fontSize);
    ImGui::StyleColorsDark(); // 设置主题

    // 初始化ImGui的GLFW和Vulkan绑定
    ImGui_ImplGlfw_InitForVulkan(mWindow, true);
    ImGui_ImplVulkan_InitInfo initInfo{};
    auto vulkanContext = injector.create<std::shared_ptr<VulkanContext>>();
    initInfo.Instance = vulkanContext->GetInstance();
    initInfo.PhysicalDevice = vulkanContext->GetPhysicalDevice();
    initInfo.Device = vulkanContext->GetDevice();
    initInfo.QueueFamily = vulkanContext->GetQueueFamilyIndicates().graphicsFamily.value();
    initInfo.Queue = vulkanContext->GetGraphicsQueue();
    initInfo.RenderPass = mUIRenderPass.get();
    initInfo.MinImageCount = 2;
    initInfo.ImageCount = mFrameCount;
    initInfo.DescriptorPoolSize = 1000;
    if (!ImGui_ImplVulkan_Init(&initInfo))
    {
        LogError("Failed to initialize ImGui Vulkan backend");
        throw std::runtime_error("ImGui Vulkan init failed");
    }
    LogDebug("Initializing ImGui with Vulkan");
}
void MEngineEditor::Shutdown()
{
    auto vulkanContext = injector.create<std::shared_ptr<VulkanContext>>();
    vulkanContext->GetDevice().waitIdle();
    if (!ImGui::GetCurrentContext())
    {
        LogWarn("ImGui context already destroyed");
        return;
    }
    if (ImGui::GetIO().BackendRendererUserData)
    {
        ImGui_ImplVulkan_Shutdown();
    }
    else
    {
        LogWarn("Vulkan renderer backend not initialized or already shutdown");
    }
    if (ImGui::GetIO().BackendPlatformUserData)
    {
        ImGui_ImplGlfw_Shutdown();
    }
    ImGuiIO &io = ImGui::GetIO();
    if (io.Fonts)
    {
        io.Fonts->Clear();
        io.Fonts->ClearInputData();
        io.Fonts->ClearTexData();
    }
    ImGui::DestroyContext();
    if (mWindow)
    {
        glfwDestroyWindow(mWindow);
        mWindow = nullptr;
    }
    mIsRunning = false;
    LogInfo("MEngine Editor shutdown successfully");
}
void MEngineEditor::CreateFramebuffer()
{
    auto vulkanContext = injector.create<std::shared_ptr<VulkanContext>>();
    const auto &swapchainImageViews = vulkanContext->GetSwapchainImageViews();
    for (const auto &imageView : swapchainImageViews)
    {
        vk::FramebufferCreateInfo framebufferInfo{};
        framebufferInfo.setRenderPass(mUIRenderPass.get())
            .setWidth(mWindowConfig.width)
            .setHeight(mWindowConfig.height)
            .setLayers(1);
        std::vector<vk::ImageView> attachments = {imageView.get()};
        framebufferInfo.setAttachments(attachments);
        auto framebuffer = vulkanContext->GetDevice().createFramebufferUnique(framebufferInfo);
        if (!framebuffer)
        {
            LogError("Failed to create UI framebuffer");
            throw std::runtime_error("Failed to create UI framebuffer");
        }
        mUIFramebuffers.push_back(std::move(framebuffer));
    }
    LogDebug("UI framebuffer created successfully");
}
void MEngineEditor::CreateCommandBuffers()
{
    auto vulkanContext = injector.create<std::shared_ptr<VulkanContext>>();

    vk::CommandBufferAllocateInfo allocInfo{};
    allocInfo.setCommandPool(vulkanContext->GetGraphicsCommandPool())
        .setLevel(vk::CommandBufferLevel::ePrimary)
        .setCommandBufferCount(mFrameCount);
    mUICmdBuffers = vulkanContext->GetDevice().allocateCommandBuffersUnique(allocInfo);
    LogDebug("UI command buffer allocated successfully");
}
void MEngineEditor::CreateUIRenderPass()
{
    auto vulkanContext = injector.create<std::shared_ptr<VulkanContext>>();
    vk::RenderPassCreateInfo renderPassInfo{};
    std::array<vk::AttachmentDescription, 1> attachments = {
        vk::AttachmentDescription()
            .setFormat(vulkanContext->GetSurfaceInfo().format.format)
            .setSamples(vk::SampleCountFlagBits::e1)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore)
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setFinalLayout(vk::ImageLayout::ePresentSrcKHR)};
    std::array<vk::AttachmentReference, 2> colorReferences = {
        vk::AttachmentReference().setAttachment(0).setLayout(vk::ImageLayout::eColorAttachmentOptimal),
    };
    renderPassInfo.setAttachments(attachments)
        .setSubpasses(vk::SubpassDescription()
                          .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
                          .setColorAttachments(colorReferences[0]));
    mUIRenderPass = vulkanContext->GetDevice().createRenderPassUnique(renderPassInfo);
    if (!mUIRenderPass)
    {
        LogError("Failed to create UI render pass");
        throw std::runtime_error("Failed to create UI render pass");
    }
    LogDebug("UI render pass created successfully");
}
void MEngineEditor::CreateFences()
{
    auto vulkanContext = injector.create<std::shared_ptr<VulkanContext>>();
    vk::FenceCreateInfo fenceInfo{};
    fenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
    mInFlightFences.reserve(mFrameCount);
    for (size_t i = 0; i < mFrameCount; ++i)
    {
        auto fence = vulkanContext->GetDevice().createFenceUnique(fenceInfo);
        if (!fence)
        {
            LogError("Failed to create UI fence");
            throw std::runtime_error("Failed to create UI fence");
        }
        mInFlightFences.push_back(std::move(fence));
    }
    LogDebug("UI fences created successfully");
}
void MEngineEditor::CreateSemaphores()
{
    auto vulkanContext = injector.create<std::shared_ptr<VulkanContext>>();
    vk::SemaphoreCreateInfo semaphoreInfo{};
    mImageAvailableSemaphores.reserve(mFrameCount);
    mRenderFinishedSemaphores.reserve(mFrameCount);
    for (size_t i = 0; i < mFrameCount; ++i)
    {
        auto imageAvailableSemaphore = vulkanContext->GetDevice().createSemaphoreUnique(semaphoreInfo);
        if (!imageAvailableSemaphore)
        {
            LogError("Failed to create image available semaphore");
            throw std::runtime_error("Failed to create image available semaphore");
        }
        mImageAvailableSemaphores.push_back(std::move(imageAvailableSemaphore));

        auto renderFinishedSemaphore = vulkanContext->GetDevice().createSemaphoreUnique(semaphoreInfo);
        if (!renderFinishedSemaphore)
        {
            LogError("Failed to create render finished semaphore");
            throw std::runtime_error("Failed to create render finished semaphore");
        }
        mRenderFinishedSemaphores.push_back(std::move(renderFinishedSemaphore));
    }
    LogDebug("UI semaphores created successfully");
}
void MEngineEditor::UI()
{
    ImGuiViewport *viewport = ImGui::GetMainViewport();

    mDockSpaceID = ImGui::DockSpaceOverViewport();

    // 3. 首次运行时初始化布局
    static bool first_run = true;
    if (first_run)
    {
        first_run = false;
        ImGui::DockBuilderRemoveNode(mDockSpaceID);
        ImGui::DockBuilderAddNode(mDockSpaceID, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(mDockSpaceID, viewport->WorkSize);
        // 1. 主区域拆分为底部（30%）和顶部（70%）
        ImGuiID dockBottomID, dockTopID;
        ImGui::DockBuilderSplitNode(mDockSpaceID, ImGuiDir_Down, 0.3, &dockBottomID, &dockTopID);
        // 2. 顶部区域拆分为左（30%）和剩余部分（70%）
        ImGuiID dockLeftID, remainingTop;
        ImGui::DockBuilderSplitNode(dockTopID, ImGuiDir_Left, 0.3, &dockLeftID, &remainingTop);
        // 3. 剩余部分（70%）拆分为中（60%）和右（40%）
        ImGuiID dockCenterID, dockRightID;
        ImGui::DockBuilderSplitNode(remainingTop, ImGuiDir_Right, 0.4, &dockRightID, &dockCenterID);
        // 4. 中部拆分上(20%)和下(80%)
        ImGuiID dockTopCenterID, dockBottomCenterID;
        ImGui::DockBuilderSplitNode(dockCenterID, ImGuiDir_Up, 0.2, &dockTopCenterID, &dockBottomCenterID);
        // 绑定窗口
        ImGui::DockBuilderDockWindow("Viewport", dockBottomCenterID); // 中间
        ImGui::DockBuilderDockWindow("Hierarchy", dockLeftID);        // 左侧
        ImGui::DockBuilderDockWindow("Inspector", dockRightID);       // 右侧
        ImGui::DockBuilderDockWindow("Assets", dockBottomID);         // 底部
        ImGui::DockBuilderDockWindow("Toolbar", dockTopCenterID);     // 顶部
        ImGui::DockBuilderFinish(mDockSpaceID);
    }
    RenderInspectorPanel();
    RenderAssetPanel();
    RenderViewportPanel();
    RenderToolbarPanel();
    RenderHierarchyPanel();
}

void MEngineEditor::RenderToolbarPanel()
{
    ImGui::Begin("Toolbar", nullptr, ImGuiWindowFlags_None);
    ImGui::End();
}
void MEngineEditor::RenderViewportPanel()
{
    ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_None);
    ImGui::Text("Viewport Panel");
    ImGui::End();
}
void MEngineEditor::RenderHierarchyPanel()
{
    ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_None);
    ImGui::Text("Hierarchy Panel");
    ImGui::End();
}
void MEngineEditor::RenderInspectorPanel()
{
    ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_None);
    ImGui::Text("Inspector Panel");
    ImGui::End();
}
void MEngineEditor::RenderAssetPanel()
{
    ImGui::Begin("Assets", nullptr, ImGuiWindowFlags_None);
    ImGui::Text("Asset Panel");
    ImGui::End();
}
} // namespace MEngine::Editor