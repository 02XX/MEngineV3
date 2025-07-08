#include "MEngineEditor.hpp"
#include "AssetDatabase.hpp"
#include "Configure.hpp"
#include "EditorSerialize.hpp"
#include "IMMeshManager.hpp"
#include "IMModelManager.hpp"
#include "Logger.hpp"
#include "MAsset.hpp"
#include "MCameraComponent.hpp"
#include "MCameraSystem.hpp"
#include "MFolderManager.hpp"
#include "MLightComponent.hpp"
#include "MMaterialComponent.hpp"
#include "MMesh.hpp"
#include "MMeshComponent.hpp"
#include "MMeshManager.hpp"
#include "MModelManager.hpp"
#include "MPBRMaterial.hpp"
#include "MPBRMaterialManager.hpp"
#include "MPipeline.hpp"
#include "MPipelineManager.hpp"
#include "MRenderSystem.hpp"
#include "MTexture.hpp"
#include "MTextureManager.hpp"

#include "EntityUtils.hpp"
#include "ImageUtil.hpp"
#include "MTransformComponent.hpp"
#include "MTransformSystem.hpp"
#include "Math.hpp"
#include "RenderPassManager.hpp"
#include "TaskManager.hpp"
#include "UUIDGenerator.hpp"
#include "VulkanContext.hpp"
#include <ImGuizmo.h>
#include <boost/di.hpp>
#include <cstddef>
#include <cstdint>
#include <entt/entt.hpp>
#include <entt/meta/meta.hpp>
#include <entt/meta/resolve.hpp>
#include <filesystem>
#include <functional>
#include <glm/ext/matrix_transform.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <imgui_internal.h>
#include <memory>

namespace MEngine::Editor
{

namespace DI = boost::di;
auto injector = DI::make_injector(

    DI::bind<IConfigure>().to<Configure>().in(DI::unique),
    DI::bind<VulkanContext>().to<VulkanContext>().in(DI::singleton),
    DI::bind<AssetDatabase>().to<AssetDatabase>().in(DI::singleton),
    DI::bind<ResourceManager>().to<ResourceManager>().in(DI::singleton),
    DI::bind<IMTextureManager>().to<MTextureManager>().in(DI::singleton),
    DI::bind<IMFolderManager>().to<MFolderManager>().in(DI::singleton),
    DI::bind<IMPipelineManager>().to<MPipelineManager>().in(DI::singleton),
    DI::bind<IMModelManager>().to<MModelManager>().in(DI::singleton),
    DI::bind<IMMeshManager>().to<MMeshManager>().in(DI::singleton),
    DI::bind<IMPBRMaterialManager>().to<MPBRMaterialManager>().in(DI::singleton),
    DI::bind<RenderPassManager>().to<RenderPassManager>().in(DI::singleton),
    DI::bind<IUUIDGenerator>().to<UUIDGenerator>().in(DI::singleton),
    DI::bind<entt::registry>().to<entt::registry>().in(DI::singleton),
    DI::bind<MRenderSystem>().to<MRenderSystem>().in(DI::singleton),
    DI::bind<MTransformSystem>().to<MTransformSystem>().in(DI::singleton),
    DI::bind<MCameraSystem>().to<MCameraSystem>().in(DI::singleton)

);
void MEngineEditor::Init()
{

    InitWindow();
    InitVulkan();
    InitImGui();
    InitDataBase();
    RegisterMeta();
    InitEditorCamera();
    InitSystem();
    mIsRunning = true;
    auto modelManager = injector.create<std::shared_ptr<IMModelManager>>();
    auto cube = modelManager->CreateCube();
    auto registry = injector.create<std::shared_ptr<entt::registry>>();
    auto cubeGameObject = Function::Utils::EntityUtils::CreateEntity(registry, cube);
    auto &cubeTransform = registry->get<MTransformComponent>(cubeGameObject);
    cubeTransform.localPosition = glm::vec3(-2.0f, 0.0f, 0.0f);
    auto lightGameObject = registry->create();
    auto &lightTransform = registry->emplace<MTransformComponent>(lightGameObject, MTransformComponent{});
    auto &lightComponent = registry->emplace<MLightComponent>(lightGameObject, MLightComponent{});
    lightComponent.LightType = LightType::Directional;
    lightTransform.name = "Directional Light";
    lightTransform.localRotation = glm::quat(glm::vec3(0.0, -glm::half_pi<float>(), 0.0f));
    LogInfo("MEngine Editor initialized successfully");
}
void MEngineEditor::InitWindow()
{
    // 读取配置文件
    mWindowConfig = injector.create<std::shared_ptr<IConfigure>>()->GetJson().get<WindowConfig>();
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
    io.FontDefault = io.Fonts->AddFontFromFileTTF("Assets/Fonts/NotoSans-Medium.ttf", 18.0f);
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
    // 加载UI图标
    auto textureManager = injector.create<std::shared_ptr<IMTextureManager>>();
    auto loadIcon = [textureManager, this](const std::string &path, MAssetType type) {
        auto textureSetting = MTextureSetting{};
        textureSetting.width = 200;
        textureSetting.height = 200;
        textureSetting.isShaderResource = true;
        mAssetIconTextures[type] =
            textureManager->Create(textureSetting, "AssetIcon_" + std::string(magic_enum::enum_name(type)));
        auto [W, H, C, data] = Utils::ImageUtil::LoadImage(path);
        textureManager->Write(
            mAssetIconTextures[type], data,
            TextureSize{static_cast<uint32_t>(W), static_cast<uint32_t>(H), static_cast<uint32_t>(C)});
        mAssetIcons[type] = ImGui_ImplVulkan_AddTexture(
            mAssetIconTextures[type]->GetSampler(), mAssetIconTextures[type]->GetImageView(),
            static_cast<VkImageLayout>(vk::ImageLayout::eShaderReadOnlyOptimal));
    };
    loadIcon("Assets/Icons/folder.png", MAssetType::Folder);
    loadIcon("Assets/Icons/texture.png", MAssetType::Texture);
    loadIcon("Assets/Icons/material.png", MAssetType::Material);
    loadIcon("Assets/Icons/model.png", MAssetType::Mesh);
    loadIcon("Assets/Icons/shader.png", MAssetType::Shader);
    loadIcon("Assets/Icons/file.png", MAssetType::File);
    loadIcon("Assets/Icons/audio.png", MAssetType::Audio);
    loadIcon("Assets/Icons/animation.png", MAssetType::Animation);
    loadIcon("Assets/Icons/script.png", MAssetType::Script);

    ImGuizmo::SetImGuiContext(ImGui::GetCurrentContext());
    ImGuizmo::Enable(true);
    LogDebug("Initializing ImGui with Vulkan");
}
void MEngineEditor::InitDataBase()
{
    mAssetDatabase = injector.create<std::shared_ptr<AssetDatabase>>();
    mRootFolder = mAssetDatabase->LoadDatabase(mProjectPath);
    mCurrentFolder = mRootFolder;
}
void MEngineEditor::InitEditorCamera()
{
    auto registry = injector.create<std::shared_ptr<entt::registry>>();
    mEditorCameraEntity = registry->create();
    auto &cameraMTransformComponent =
        registry->emplace<MTransformComponent>(mEditorCameraEntity, MTransformComponent{});
    cameraMTransformComponent.name = "EditorCamera";
    cameraMTransformComponent.localPosition = glm::vec3(0.0f, 0.0f, 5.0f);
    auto &cameraComponent = registry->emplace<MCameraComponent>(mEditorCameraEntity, MCameraComponent{});
    cameraComponent.isEditorCamera = true;
    cameraComponent.isMainCamera = true;
}
void MEngineEditor::InitSystem()
{
    auto cameraSystem = injector.create<std::shared_ptr<MCameraSystem>>();
    cameraSystem->Init();
    auto transformSystem = injector.create<std::shared_ptr<MTransformSystem>>();
    transformSystem->Init();

    mRenderSystem = injector.create<std::shared_ptr<MRenderSystem>>();
    mRenderSystem->SetFrameCount(mFrameCount);
    mRenderSystem->SetExtent(800, 600);
    mRenderSystem->Init();
    SetViewPort();
}
void MEngineEditor::SetViewPort()
{
    for (auto &viewPortDescriptorSet : mViewPortDescriptorSets)
    {
        ImGui_ImplVulkan_RemoveTexture(viewPortDescriptorSet);
    }
    mViewPortDescriptorSets.clear();
    mViewPortDescriptorSets.resize(mFrameCount);
    for (uint32_t i = 0; i < mFrameCount; ++i)
    {
        auto &renderTarget = mRenderSystem->GetRenderTarget(i);
        mViewPortDescriptorSets[i] = ImGui_ImplVulkan_AddTexture(
            renderTarget.colorTexture->GetSampler(), renderTarget.colorTexture->GetImageView(),
            static_cast<VkImageLayout>(vk::ImageLayout::eShaderReadOnlyOptimal));
    }

    auto registry = injector.create<std::shared_ptr<entt::registry>>();
    auto view = registry->view<MCameraComponent>();
    for (auto entity : view)
    {
        auto &cameraComponent = registry->get<MCameraComponent>(entity);
        if (cameraComponent.isMainCamera)
        {
            cameraComponent.aspectRatio = static_cast<float>(mCurrentResolution.width) / mCurrentResolution.height;
            cameraComponent.dirty = true;
            break;
        }
    }
}
void MEngineEditor::Run(float deltaTime)
{
    auto vulkanContext = injector.create<std::shared_ptr<VulkanContext>>();
    auto cameraSystem = injector.create<std::shared_ptr<MCameraSystem>>();
    auto transformSystem = injector.create<std::shared_ptr<MTransformSystem>>();
    // auto cameraSystemTask = mTaskflow.emplace([this, cameraSystem, deltaTime]() {
    //     while (mIsRunning)
    //     {
    //         cameraSystem->Update(deltaTime);
    //     }
    // });
    // auto transformSystemTask = mTaskflow.emplace([this, transformSystem, deltaTime]() {
    //     while (mIsRunning)
    //     {
    //         transformSystem->Update(deltaTime);
    //     }
    // });
    auto &executor = Thread::TaskManager::GetExecutor();
    executor.run(mTaskflow);
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
            HandleSwapchainOutOfDate();
            continue;
        }

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        UI();
        if (mIsRunning)
        {
            transformSystem->Update(deltaTime);
            cameraSystem->Update(deltaTime);
            mRenderSystem->Update(deltaTime);
        }

        ImGui::Render();
        mUICmdBuffers[mCurrentFrameIndex]->reset();
        mUICmdBuffers[mCurrentFrameIndex]->begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
        // 转换图像布局
        auto &renderTarget = mRenderSystem->GetRenderTarget(mCurrentFrameIndex);
        vk::ImageMemoryBarrier preBarrier;
        preBarrier.setImage(renderTarget.colorTexture->GetImage())
            .setOldLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
            .setSrcQueueFamilyIndex(vulkanContext->GetQueueFamilyIndicates().graphicsFamily.value())
            .setDstQueueFamilyIndex(vulkanContext->GetQueueFamilyIndicates().graphicsFamily.value())
            .setSubresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1))
            .setSrcAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
            .setDstAccessMask(vk::AccessFlagBits::eShaderRead);
        mUICmdBuffers[mCurrentFrameIndex]->pipelineBarrier(vk::PipelineStageFlagBits::eColorAttachmentOutput,
                                                           vk::PipelineStageFlagBits::eFragmentShader, {}, {}, {},
                                                           preBarrier);
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
        vk::ImageMemoryBarrier postBarrier;
        postBarrier.setImage(renderTarget.colorTexture->GetImage())
            .setOldLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
            .setNewLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setSrcQueueFamilyIndex(vulkanContext->GetQueueFamilyIndicates().graphicsFamily.value())
            .setDstQueueFamilyIndex(vulkanContext->GetQueueFamilyIndicates().graphicsFamily.value())
            .setSubresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1))
            .setSrcAccessMask(vk::AccessFlagBits::eShaderRead)
            .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);
        mUICmdBuffers[mCurrentFrameIndex]->pipelineBarrier(vk::PipelineStageFlagBits::eFragmentShader,
                                                           vk::PipelineStageFlagBits::eColorAttachmentOutput, {}, {},
                                                           {}, postBarrier);
        mUICmdBuffers[mCurrentFrameIndex]->end();
        vk::SubmitInfo submitInfo;

        auto waitSemaphores = {mImageAvailableSemaphores[mCurrentFrameIndex].get(),
                               mRenderSystem->GetRenderFinishedSemaphore(mCurrentFrameIndex)};
        auto signalSemaphores = {mRenderFinishedSemaphores[mCurrentFrameIndex].get()};
        std::vector<vk::PipelineStageFlags> waitStage = {vk::PipelineStageFlagBits::eColorAttachmentOutput,
                                                         vk::PipelineStageFlagBits::eFragmentShader};
        submitInfo.setCommandBuffers(mUICmdBuffers[mCurrentFrameIndex].get())
            .setSignalSemaphores(signalSemaphores)
            .setWaitSemaphores(waitSemaphores)
            .setWaitDstStageMask({waitStage});

        vulkanContext->GetGraphicsQueue().submit(submitInfo, mInFlightFences[mCurrentFrameIndex].get());

        vk::PresentInfoKHR presentInfo;
        auto presentWaitSemaphores = {mRenderFinishedSemaphores[mCurrentFrameIndex].get()};
        presentInfo.setSwapchains(vulkanContext->GetSwapchain())
            .setImageIndices({resultValue.value})
            .setWaitSemaphores(presentWaitSemaphores);
        try
        {
            auto presentResult = vulkanContext->GetPresentQueue().presentKHR(presentInfo);
            if (presentResult != vk::Result::eSuccess && presentResult != vk::Result::eSuboptimalKHR)
            {
                LogError("Failed to present image: {}", vk::to_string(presentResult));
                throw std::runtime_error("Failed to present image");
            }
        }
        catch (vk::OutOfDateKHRError &)
        {
            HandleSwapchainOutOfDate();
        }
        mCurrentFrameIndex = (mCurrentFrameIndex + 1) % mFrameCount;
    }
    // executor.wait_for_all();
    LogDebug("MEngine Editor run loop exited");
}
void MEngineEditor::HandleSwapchainOutOfDate()
{
    auto vulkanContext = injector.create<std::shared_ptr<VulkanContext>>();
    vulkanContext->GetDevice().waitIdle();
    vulkanContext->RecreateSwapchain();
    auto &surfaceInfo = vulkanContext->GetSurfaceInfo();
    mWindowConfig.width = surfaceInfo.extent.width;
    mWindowConfig.height = surfaceInfo.extent.height;
    CreateFramebuffer();
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
    mRenderSystem->Shutdown();
    mIsRunning = false;
    LogInfo("MEngine Editor shutdown successfully");
}
void MEngineEditor::CreateFramebuffer()
{
    mUIFramebuffers.clear();
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
    RenderToolbarPanel();
    RenderViewportPanel();
    RenderHierarchyPanel();
}
void MEngineEditor::RenderToolbarPanel()
{
    ImGui::Begin("Toolbar", nullptr, ImGuiWindowFlags_None);
    ImGui::BeginGroup();
    {
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "FPS: %1.f", ImGui::GetIO().Framerate);
        if (ImGui::RadioButton("Translate", mGuizmoOperation == ImGuizmo::TRANSLATE) || ImGui::IsKeyDown(ImGuiKey_W))
            mGuizmoOperation = ImGuizmo::TRANSLATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Rotate", mGuizmoOperation == ImGuizmo::ROTATE) || ImGui::IsKeyDown(ImGuiKey_E))
            mGuizmoOperation = ImGuizmo::ROTATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Scale", mGuizmoOperation == ImGuizmo::SCALE) || ImGui::IsKeyDown(ImGuiKey_R))
            mGuizmoOperation = ImGuizmo::SCALE;
        if (ImGui::RadioButton("Local", mGuizmoMode == ImGuizmo::LOCAL))
            mGuizmoMode = ImGuizmo::LOCAL;
        ImGui::SameLine();
        if (ImGui::RadioButton("World", mGuizmoMode == ImGuizmo::WORLD))
            mGuizmoMode = ImGuizmo::WORLD;
        ImGui::EndGroup();
    }
    ImGui::BeginGroup();
    {
        if (ImGui::BeginCombo("Resolution", mCurrentResolution.ToString().c_str()))
        {
            for (Resolution &resolution : mResolutions)
            {
                if (ImGui::Selectable(resolution.ToString().data(), mCurrentResolution == resolution))
                {
                    mCurrentResolution = resolution;
                    mRenderSystem->ReSizeFrameBuffer(resolution.width, resolution.height);
                    SetViewPort();
                }
            }
            ImGui::EndCombo();
        }
        ImGui::EndGroup();
    }
    ImGui::End();
}
void MEngineEditor::RenderViewportPanel()
{
    ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_None);
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 windowSize = ImGui::GetContentRegionAvail();
    ImVec2 viewPortSize = {windowSize.x, windowSize.x * mCurrentResolution.height * 1.0f / mCurrentResolution.width};
    ImGui::Image(reinterpret_cast<ImTextureID>(mViewPortDescriptorSets[mCurrentFrameIndex]), viewPortSize, ImVec2(0, 1),
                 ImVec2(1, 0));
    // 设置 ImGuizmo 绘制区域
    ImVec2 imagePos = ImGui::GetItemRectMin();
    ImVec2 imageSize = ImGui::GetItemRectSize();
    ImGuizmo::SetRect(imagePos.x, imagePos.y, imageSize.x, imageSize.y);
    ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
    if (mSelectedEntity != entt::null)
    {
        auto registry = injector.create<std::shared_ptr<entt::registry>>();
        auto &cameraComponent = registry->get<MCameraComponent>(mEditorCameraEntity);
        auto viewMatrix = cameraComponent.viewMatrix;
        auto projectionMatrix = cameraComponent.projectionMatrix;
        if (registry->any_of<MTransformComponent>(mSelectedEntity))
        {
            auto &transform = registry->get<MTransformComponent>(mSelectedEntity);
            auto modelMatrix = transform.modelMatrix;
            ImGuizmo::Manipulate(glm::value_ptr(viewMatrix), glm::value_ptr(projectionMatrix), mGuizmoOperation,
                                 mGuizmoMode, glm::value_ptr(modelMatrix));
            if (ImGuizmo::IsUsing())
            {
                // 分解矩阵
                glm::vec3 translation, scale, skew;
                glm::quat rotation;
                glm::vec4 perspective;
                if (glm::decompose(modelMatrix, scale, rotation, translation, skew, perspective))
                {
                    transform.localPosition = translation;
                    transform.localRotation = rotation;
                    transform.localScale = scale;

                    transform.dirty = true;
                }
            }
        }
        if (registry->all_of<MTransformComponent, MLightComponent>(mSelectedEntity))
        {
            auto &transformComponent = registry->get<MTransformComponent>(mSelectedEntity);
            auto &lightComponent = registry->get<MLightComponent>(mSelectedEntity);
            auto drawList = ImGui::GetWindowDrawList();
            switch (lightComponent.LightType)
            {
            case Function::Component::LightType::Directional: {
                auto modelMatrix = transformComponent.modelMatrix;
                auto worldPositionStart4D = glm::vec4(transformComponent.worldPosition, 1.0f);
                auto worldPositionEnd4D = glm::vec4(
                    transformComponent.worldPosition + transformComponent.worldRotation * glm::vec3(0, 0, 1), 1.0f);
                auto NDCStartPosition = projectionMatrix * viewMatrix * worldPositionStart4D;
                auto NDCEndPosition = projectionMatrix * viewMatrix * worldPositionEnd4D;
                NDCStartPosition /= NDCStartPosition.w; // 转换为NDC坐标
                NDCEndPosition /= NDCEndPosition.w;     // 转换为NDC坐标
                ImVec2 startPos = ImVec2((NDCStartPosition.x + 1.0f) * 0.5f * imageSize.x + imagePos.x,
                                         (1.0f - (NDCStartPosition.y + 1.0f) * 0.5f) * imageSize.y + imagePos.y);
                ImVec2 endPos = ImVec2((NDCEndPosition.x + 1.0f) * 0.5f * imageSize.x + imagePos.x,
                                       (1.0f - (NDCEndPosition.y + 1.0f) * 0.5f) * imageSize.y + imagePos.y);
                // 绘制光源方向线
                drawList->AddLine(startPos, endPos, IM_COL32(255, 255, 0, 255), 2.0f);
                // 绘制方向箭头（三角形）
                float arrowSize = 8.0f;
                ImVec2 dir = ImVec2(endPos.x - startPos.x, endPos.y - startPos.y);
                float dirLength = sqrtf(dir.x * dir.x + dir.y * dir.y);
                if (dirLength > 0.0f)
                {
                    dir.x /= dirLength; // 归一化
                    dir.y /= dirLength;

                    // 计算箭头两个侧边点
                    ImVec2 arrowLeft = ImVec2(endPos.x - dir.x * arrowSize + dir.y * arrowSize * 0.5f,
                                              endPos.y - dir.y * arrowSize - dir.x * arrowSize * 0.5f);
                    ImVec2 arrowRight = ImVec2(endPos.x - dir.x * arrowSize - dir.y * arrowSize * 0.5f,
                                               endPos.y - dir.y * arrowSize + dir.x * arrowSize * 0.5f);

                    // 绘制填充三角形（黄色）
                    drawList->AddTriangleFilled(endPos, arrowLeft, arrowRight, IM_COL32(255, 255, 0, 255));
                }
                break;
            }
            case Function::Component::LightType::Point:
            case Function::Component::LightType::Spot:
                break;
            }
        }
    }
    ImGui::End();
}
void MEngineEditor::RenderHierarchyPanel()
{
    ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_None);
    auto registry = injector.create<std::shared_ptr<entt::registry>>();
    ImGui::BeginChild("HierarchyList");
    {
        std::function<void(const entt::entity &)> renderEntity = [this, &renderEntity,
                                                                  registry](const entt::entity &entity) {
            auto &transform = registry->get<MTransformComponent>(entity);
            std::string name = transform.name;
            ImGuiTreeNodeFlags flags =
                ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;
            if (transform.children.empty())
            {
                flags |= ImGuiTreeNodeFlags_Leaf;
            }
            bool isSelected = (mSelectedEntity == entity);
            // 如果选中，设置背景色
            if (isSelected)
            {
                ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.2f, 0.3f, 0.8f, 1.0f));        // 选中时的背景色
                ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.3f, 0.4f, 0.9f, 1.0f)); // 悬停时的背景色
                ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.2f, 0.3f, 0.8f, 1.0f));  // 激活时的背景色
                flags |= ImGuiTreeNodeFlags_Selected;
            }
            bool opened = ImGui::TreeNodeEx((void *)(uint64_t)entity, flags, "%s", name.data());
            if (isSelected)
            {
                ImGui::PopStyleColor(3);
            }
            if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
            {
                mSelectedEntity = entity;
            }
            if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            {
                mSelectedEntity = entity;
            }
            if (ImGui::BeginDragDropSource())
            {
                ImGui::SetDragDropPayload("ENTITY_DRAG_DROP", &entity, sizeof(entt::entity));
                ImGui::Text("Move %s", name.data());
                ImGui::EndDragDropSource();
            }
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ENTITY_DRAG_DROP"))
                {
                    IM_ASSERT(payload->DataSize == sizeof(entt::entity));
                    entt::entity draggedAsset = *(const entt::entity *)payload->Data;

                    // 重新设置父子关系
                    auto &draggedTransform = registry->get<MTransformComponent>(draggedAsset);
                    auto &targetTransform = registry->get<MTransformComponent>(entity);
                    // 判断拖拽实体是否已经是目标实体的后继节点
                    MTransformComponent targetTransformRoot = targetTransform;
                    bool isDescendant = false;
                    while (targetTransformRoot.parent != entt::null)
                    {
                        if (targetTransformRoot.parent == draggedAsset)
                        {
                            isDescendant = true;
                            break;
                        }
                        targetTransformRoot = registry->get<MTransformComponent>(targetTransformRoot.parent);
                    }
                    if (!isDescendant)
                    {
                        // 移除拖拽实体的父节点
                        if (draggedTransform.parent != entt::null)
                        {
                            auto &parentTransform = registry->get<MTransformComponent>(draggedTransform.parent);
                            auto it = std::find(parentTransform.children.begin(), parentTransform.children.end(),
                                                draggedAsset);
                            if (it != parentTransform.children.end())
                            {
                                parentTransform.children.erase(it);
                            }
                        }
                        // 设置新的父节点
                        draggedTransform.parent = entity;
                        targetTransform.children.push_back(draggedAsset);
                    }
                }
                ImGui::EndDragDropTarget();
            }

            // 递归绘制子节点
            if (opened)
            {
                for (auto child : transform.children)
                {
                    renderEntity(child);
                }
                ImGui::TreePop();
            }
        };
        auto view = registry->view<MTransformComponent>();
        std::vector<entt::entity> rootEntities;
        for (auto entity : view)
        {
            auto &transform = view.get<MTransformComponent>(entity);
            if (transform.parent == entt::null)
            {
                rootEntities.push_back(entity);
            }
        }
        for (auto entity : rootEntities)
        {
            renderEntity(entity);
        }
        // 右键菜单
        if (ImGui::BeginPopupContextWindow("HierarchyContextMenu", ImGuiPopupFlags_MouseButtonRight))
        {
            if (ImGui::MenuItem("Create Empty"))
            {
                auto entity = registry->create();
                auto &transform = registry->emplace<MTransformComponent>(entity);
            }
            if (mSelectedEntity != entt::null && registry->valid(mSelectedEntity))
            {
                if (ImGui::MenuItem("Delete"))
                {
                    // mSelectedEntity = entt::null;
                }
            }
            ImGui::EndPopup();
        }
    }
    ImGui::EndChild();
    ImGui::End();
}
void MEngineEditor::RenderInspectorPanel()
{
    ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_None);
    auto vulkanContext = injector.create<std::shared_ptr<VulkanContext>>();
    if (ImGui::BeginTabBar("InspectorTabs"))
    {
        if (ImGui::BeginTabItem("Components"))
        {
            ImGui::Text("Components Panel");
            auto registry = injector.create<std::shared_ptr<entt::registry>>();
            if (mSelectedEntity != entt::null && registry->valid(mSelectedEntity))
            {
                auto registry = injector.create<std::shared_ptr<entt::registry>>();
                if (registry->any_of<MTransformComponent>(mSelectedEntity))
                {
                    auto &transform = registry->get<MTransformComponent>(mSelectedEntity);
                    auto metaTransform = entt::forward_as_meta(transform);
                    ReflectObject(metaTransform, entt::resolve<MTransformComponent>());
                }
                if (registry->any_of<MCameraComponent>(mSelectedEntity))
                {
                    auto &camera = registry->get<MCameraComponent>(mSelectedEntity);
                    auto metaCamera = entt::forward_as_meta(camera);
                    ReflectObject(metaCamera, entt::resolve<MCameraComponent>());
                }
                if (registry->any_of<MMeshComponent>(mSelectedEntity))
                {
                    auto &mesh = registry->get<MMeshComponent>(mSelectedEntity);
                    auto metaMesh = entt::forward_as_meta(mesh);
                    ReflectObject(metaMesh, entt::resolve<MMeshComponent>());
                }
                if (registry->any_of<MMaterialComponent>(mSelectedEntity))
                {
                    auto &materialComponent = registry->get<MMaterialComponent>(mSelectedEntity);
                    auto metaMaterial = entt::forward_as_meta(materialComponent);
                    if (ReflectObject(metaMaterial, entt::resolve<MMaterialComponent>()))
                    {
                        vulkanContext->GetDevice().waitIdle();
                        switch (materialComponent.material->GetMaterialType())
                        {
                        case Core::Asset::MMaterialType::Unknown:
                        case Core::Asset::MMaterialType::PBR: {
                            auto pbrMaterialManager = injector.create<std::shared_ptr<MPBRMaterialManager>>();
                            if (auto pbrMaterial = std::dynamic_pointer_cast<MPBRMaterial>(materialComponent.material))
                            {
                                pbrMaterialManager->Write(pbrMaterial);
                            }
                            break;
                        }
                        case Core::Asset::MMaterialType::Unlit:
                        case Core::Asset::MMaterialType::Custom:
                            break;
                        }
                    }
                }
                if (registry->any_of<MLightComponent>(mSelectedEntity))
                {
                    auto &light = registry->get<MLightComponent>(mSelectedEntity);
                    auto metaLight = entt::forward_as_meta(light);
                    if (ReflectObject(metaLight, entt::resolve<MLightComponent>()))
                    {
                    }
                }
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Assets"))
        {
            if (mSelectedAsset != nullptr)
            {
                switch (mSelectedAsset->GetType())
                {
                case MAssetType::Texture: {
                    auto metaAsset = entt::forward_as_meta(*std::static_pointer_cast<MTexture>(mSelectedAsset));
                    ReflectObject(metaAsset, entt::resolve<MTexture>());
                    break;
                }
                case MAssetType::Shader: {
                    auto metaAsset = entt::forward_as_meta(*std::static_pointer_cast<MPipeline>(mSelectedAsset));
                    ReflectObject(metaAsset, entt::resolve<MPipeline>());
                    break;
                }
                default:
                    break;
                }
            }
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::End();
}
void MEngineEditor::RenderAssetPanel()
{
    ImGui::Begin("Assets", nullptr, ImGuiWindowFlags_None);
    // ==== 左侧路径和返回按钮 ====
    if (ImGui::Button("<-"))
    {
        if (mCurrentFolder->GetParentFolder() != nullptr)
        {
            mCurrentFolder = mCurrentFolder->GetParentFolder();
            mSelectedAsset = nullptr;
            mHoveredAsset = nullptr;
        }
    }
    ImGui::SameLine();
    ImGui::TextUnformatted(mCurrentFolder->GetPath().string().c_str());

    // ==== 右侧操作区（Refresh按钮 + Columns滑动条） ====
    // -- 计算所有右侧控件总宽度（包括空隙） --
    float refreshBtnWidth = ImGui::CalcTextSize("Refresh").x + ImGui::GetStyle().FramePadding.x * 2;
    float sliderLabelWidth =
        ImGui::CalcTextSize("Columns").x + ImGui::GetStyle().FramePadding.x * 2; // 其实SliderInt标签在滑动条内部
    float sliderWidth = 140.0f;                                                  // 设置滑动条宽度
    float gap = ImGui::GetStyle().ItemSpacing.x;
    float buttonWidth = ImGui::CalcTextSize("Refresh").x + ImGui::GetStyle().FramePadding.x * 2;

    float rightControlsWidth = buttonWidth + gap + sliderLabelWidth + gap + sliderWidth;

    float totalRegionWidth = ImGui::GetContentRegionAvail().x + ImGui::GetCursorPosX();
    float rightStart = totalRegionWidth - rightControlsWidth;
    ImGui::SameLine();
    ImGui::SetCursorPosX(rightStart);
    // 按钮
    if (ImGui::Button("Refresh"))
    {
        // mAssetDatabase->Refresh(mProjectPath);
    }
    ImGui::SameLine();
    ImGui::Text("Columns");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(sliderWidth);
    ImGui::SliderInt("##Columns", &mColumns, 5, 30);
    ImGui::Separator();

    if (ImGui::BeginChild("AssetList"))
    {
        int columnIndex = 0;
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        ImVec2 containerSize = ImGui::GetContentRegionAvail();
        ImVec2 cellSize = ImVec2(containerSize.x / mColumns, containerSize.x / mColumns);
        ImVec2 iconSize = ImVec2(cellSize.x * 0.8f, cellSize.y * 0.8f);
        // 绘制
        for (auto asset : mCurrentFolder->GetChildren())
        {
            ImGui::PushID(asset->GetID().ToString().c_str());
            ImGui::BeginGroup();
            {
                if (ImGui::Selectable("##selectable", mSelectedAsset == asset, ImGuiSelectableFlags_AllowDoubleClick,
                                      cellSize))
                {
                    mSelectedAsset = asset;
                    mSelectedTab = InspectorTab::ASSETS; // 切换到Assets标签
                    if (ImGui::IsMouseDoubleClicked(0))
                    {
                        if (asset->GetType() == MAssetType::Folder)
                        {
                            mCurrentFolder = std::static_pointer_cast<MFolder>(asset);
                        }
                    }
                }
                if (ImGui::IsItemHovered())
                {
                    mHoveredAsset = asset;
                }
                auto selectableRectMin = ImGui::GetItemRectMin();
                auto selectableRectMax = ImGui::GetItemRectMax();
                auto selectableRectSize = ImGui::GetItemRectSize();
                auto imageOffset = ImVec2((selectableRectSize.x - iconSize.x) * 0.5, 0);
                auto imagePos = ImVec2(selectableRectMin.x + imageOffset.x, selectableRectMin.y + imageOffset.y);
                ImGui::SetCursorScreenPos(imagePos);
                // 根据资源类型选择图标
                ImGui::Image(reinterpret_cast<ImTextureID>(mAssetIcons[asset->GetType()]), iconSize);
                auto textSize = ImGui::CalcTextSize(asset->GetPath().filename().stem().string().c_str());
                auto textOffset = ImVec2((selectableRectSize.x - textSize.x) * 0.5f, iconSize.y);
                auto textPos = ImVec2(selectableRectMin.x + textOffset.x, selectableRectMin.y + textOffset.y);
                ImGui::SetCursorScreenPos(textPos);
                float textHeight = ImGui::GetFontSize();
                // ImGui::SetWindowFontScale(10.0f/mColumns ); // 根据列数调整字体大小
                ImGui::Text("%s", asset->GetPath().filename().stem().string().c_str());
                // ImGui::SetWindowFontScale(1.0f);
            }
            ImGui::EndGroup();
            ImGui::PopID();
            columnIndex++;
            if (columnIndex % mColumns != 0)
            {
                ImGui::SameLine(0, 0);
            }
        }
        ImGui::PopStyleVar();

        // 右键菜单
        if (ImGui::BeginPopupContextWindow("AssetContextMenu"))
        {
            auto &executor = Thread::TaskManager::GetExecutor();
            if (ImGui::MenuItem("New Folder"))
            {
                auto folder = mAssetDatabase->CreateFolder(mCurrentFolder->GetPath(), "New Folder", {});
                mAssetDatabase->SaveFolder(folder);
            }
            if (ImGui::MenuItem("New Texture"))
            {
                auto textureSetting = MTextureSetting{};
                textureSetting.width = 512;
                textureSetting.height = 512;
                auto texture =
                    mAssetDatabase->CreateAsset<MTexture>(mCurrentFolder->GetPath(), "New Texture", textureSetting);
                mAssetDatabase->SaveAsset<MTexture>(texture);
            }
            if (ImGui::MenuItem("New Pipeline"))
            {
                auto pipelineSetting = MPipelineSetting{};
                pipelineSetting.VertexShaderPath = "Assets/Shaders/ForwardOpaquePBR.vert";
                pipelineSetting.FragmentShaderPath = "Assets/Shaders/ForwardOpaquePBR.frag";
                auto pipeline =
                    mAssetDatabase->CreateAsset<MPipeline>(mCurrentFolder->GetPath(), "New Pipeline", pipelineSetting);
                mAssetDatabase->SaveAsset<MPipeline>(pipeline);
            }
            ImGui::EndPopup();
        }
        ImGui::EndChild();
    }
    ImGui::End();
}
bool MEngineEditor::ReflectObject(entt::meta_any &instance, entt::meta_type type)
{
    auto *info = static_cast<Info *>(type.custom());
    if (info == nullptr || !info->Serializable)
        return false;
    auto instanceName = info->DisplayName;
    bool isModified = false;
    if (ImGui::CollapsingHeader(instanceName.data(), ImGuiTreeNodeFlags_DefaultOpen))
    {
        for (auto &&[id, field] : type.data())
        {
            auto fieldInfo = static_cast<Info *>(field.custom());
            if (fieldInfo == nullptr || !fieldInfo->Serializable)
                continue;
            auto fieldName = fieldInfo->DisplayName.c_str();
            auto fieldType = field.type();
            auto fieldValue = field.get(instance);
            // UI
            if (!fieldInfo->Editable)
                ImGui::BeginDisabled();
            if (!fieldType.is_pointer_like())
            {
                if (fieldType == entt::resolve<bool>())
                {
                    auto value = fieldValue.cast<bool>();
                    if (ImGui::Checkbox(fieldName, &value))
                    {
                        field.set(instance, value);
                        isModified = true;
                    }
                }
                else if (fieldType == entt::resolve<int>())
                {
                    auto value = fieldValue.cast<int>();
                    if (ImGui::InputInt(fieldName, &value))
                    {
                        field.set(instance, value);
                        isModified = true;
                    }
                }
                else if (fieldType == entt::resolve<float>())
                {
                    auto value = fieldValue.cast<float>();
                    if (ImGui::InputFloat(fieldName, &value))
                    {
                        field.set(instance, value);
                        isModified = true;
                    }
                }
                else if (fieldType == entt::resolve<std::string>())
                {
                    auto value = fieldValue.cast<std::string>();
                    if (ImGui::InputText(fieldName, value.data(), value.capacity() + 1))
                    {
                        field.set(instance, value);
                        isModified = true;
                    }
                }
                else if (fieldType == entt::resolve<std::filesystem::path>())
                {
                    auto path = fieldValue.cast<std::filesystem::path>();
                    auto value = path.string();
                    if (ImGui::InputText(fieldName, value.data(), value.capacity() + 1))
                    {
                        field.set(instance, std::filesystem::path(value));
                        isModified = true;
                    }
                }
                else if (fieldType == entt::resolve<UUID>())
                {
                    auto value = fieldValue.cast<UUID>().ToString();
                    ImGui::Text("%s: %s", fieldName, value.c_str());
                }
                else if (fieldType == entt::resolve<glm::vec2>())
                {
                    auto value = fieldValue.cast<glm::vec2>();
                    if (ImGui::DragFloat2(fieldName, glm::value_ptr(value), 0.01f))
                    {
                        field.set(instance, value);
                        isModified = true;
                    }
                }
                else if (fieldType == entt::resolve<glm::vec3>())
                {
                    auto value = fieldValue.cast<glm::vec3>();
                    if (ImGui::DragFloat3(fieldName, glm::value_ptr(value), 0.01f))
                    {
                        field.set(instance, value);
                        isModified = true;
                    }
                }
                else if (fieldType == entt::resolve<glm::vec4>())
                {
                    auto value = fieldValue.cast<glm::vec4>();
                    if (ImGui::DragFloat4(fieldName, glm::value_ptr(value), 0.01f))
                    {
                        field.set(instance, value);
                        isModified = true;
                    }
                }
                else if (fieldType == entt::resolve<glm::quat>())
                {
                    auto value = fieldValue.cast<glm::quat>();
                    glm::vec3 euler = glm::degrees(glm::eulerAngles(value));
                    if (ImGui::DragFloat3(fieldName, glm::value_ptr(euler), 0.01f))
                    {
                        value = glm::quat(glm::radians(euler));
                        field.set(instance, value);
                        isModified = true;
                    }
                }
                else if (fieldType == entt::resolve<MPBRMaterialProperties>())
                {
                    auto value = fieldValue.cast<MPBRMaterialProperties>();
                    auto metaProperties = entt::forward_as_meta(value);
                    if (ReflectObject(metaProperties, entt::resolve<MPBRMaterialProperties>()))
                    {
                        field.set(instance, value);
                        isModified = true;
                    }
                }
                else if (fieldType == entt::resolve<MPBRTextures>())
                {
                    auto value = fieldValue.cast<MPBRTextures>();
                    auto metaTextures = entt::forward_as_meta(value);
                    ReflectObject(metaTextures, entt::resolve<MPBRTextures>());
                }
                else
                {
                    ImGui::Text("%s: %s", fieldName, fieldType.info().name().data());
                }
            }
            else // smart pointer
            {
                if (fieldType == entt::resolve<std::shared_ptr<MMesh>>())
                {
                }
                else if (fieldType == entt::resolve<std::shared_ptr<MMaterial>>())
                {
                    auto value = fieldValue.cast<std::shared_ptr<MMaterial>>();
                    // PBR Material
                    switch (value->GetMaterialType())
                    {
                    case Core::Asset::MMaterialType::Unknown:
                    case Core::Asset::MMaterialType::PBR: {
                        auto pbrMaterial = std::static_pointer_cast<MPBRMaterial>(value);
                        auto pbrMaterialMeta = entt::forward_as_meta(*pbrMaterial);
                        if (ReflectObject(pbrMaterialMeta, entt::resolve<MPBRMaterial>()))
                        {
                            isModified = true;
                        }
                        break;
                    }
                    case Core::Asset::MMaterialType::Unlit:
                    case Core::Asset::MMaterialType::Custom:
                        break;
                    }
                }
                else if (fieldType == entt::resolve<std::shared_ptr<MTexture>>())
                {
                    auto value = fieldValue.cast<std::shared_ptr<MTexture>>();
                    if (value->GetSetting().isShaderResource)
                    {
                        ImGui::Image(value->GetImGuiTextureID(), ImVec2(100, 100));
                        ImGui::SameLine();
                        ImGui::Text("%s", fieldName);
                    }
                }
                else if (fieldType == entt::resolve<std::shared_ptr<MPipeline>>())
                {
                    auto value = fieldValue.cast<std::shared_ptr<MPipeline>>();
                }
                else
                {
                    ImGui::Text("%s: %s", fieldName, fieldType.info().name().data());
                }
            }
            if (!fieldInfo->Editable)
                ImGui::EndDisabled();
        }
        for (auto &&[id, baseType] : type.base())
        {
            isModified |= ReflectObject(instance, baseType);
        }
    }
    return isModified;
}
} // namespace MEngine::Editor
