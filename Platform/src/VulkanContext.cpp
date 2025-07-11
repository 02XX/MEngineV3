#include "VulkanContext.hpp"
#include "Logger.hpp"
#include <algorithm>
#include <set>
#include <vector>
#include <vulkan/vulkan_to_string.hpp>
namespace MEngine
{

VulkanContext::VulkanContext()
{
}
VulkanContext::~VulkanContext()
{
    Destroy();
}
void VulkanContext::InitContext(const VulkanContextConfig &config)
{
    mConfig = config;
    CreateInstance();
    PickPhysicalDevice();
}
void VulkanContext::Init()
{
    QueryQueueFamilyIndicates();
    CreateLogicalDevice();
    GetQueues();
    CreateCommandPools();
    CreateVMA();
    CreateDescriptorPool();
    if (Surface)
    {
        QuerySurfaceInfo();
        CreateSwapchain();
        CreateSwapchainImages();
        CreateSwapchainImageViews();
    }
}
void VulkanContext::Destroy()
{
    if (VmaAllocator)
    {
        vmaDestroyAllocator(VmaAllocator);
    }
}
void VulkanContext::CreateInstance()
{
    vk::InstanceCreateInfo instanceCreateInfo;
    vk::ApplicationInfo appInfo;

    Version = vk::enumerateInstanceVersion();
    auto variant = vk::apiVersionVariant(Version);
    auto major = vk::apiVersionMajor(Version);
    auto minor = vk::apiVersionMinor(Version);
    auto patch = vk::apiVersionPatch(Version);
    auto appVersion = vk::makeApiVersion(0, 0, 0, 1);
    appInfo.setPApplicationName("MEngine")
        .setApplicationVersion(appVersion)
        .setPEngineName({})
        .setEngineVersion({})
        .setApiVersion(Version);
    // 获取所有可用的实例扩展
    auto availableExtensions = vk::enumerateInstanceExtensionProperties();
    for (auto &ext : availableExtensions)
    {
        LogTrace("Available Vulkan instance extension: {}", ext.extensionName.data());
    }

    for (auto &ext : mConfig.InstanceRequiredExtensions)
    {
        if (std::ranges::find_if(availableExtensions, [&ext](const vk::ExtensionProperties &availableExt) {
                return std::string(availableExt.extensionName) == ext;
            }) == availableExtensions.end())
        {
            LogError("Vulkan instance extension {} is not available", ext);
            throw std::runtime_error("Vulkan instance extension not available");
        }
    }
    instanceCreateInfo.setFlags({})
        .setPApplicationInfo(&appInfo)
        .setPEnabledLayerNames(mConfig.InstanceRequiredLayers)
        .setPEnabledExtensionNames(mConfig.InstanceRequiredExtensions);
    Instance = vk::createInstanceUnique(instanceCreateInfo);
    if (!Instance)
    {
        LogError("Failed to create Vulkan instance");
        throw std::runtime_error("Failed to create Vulkan instance");
    }
    LogTrace("Vulkan instance created with version: {}.{}.{}.{}", variant, major, minor, patch);
    LogTrace("Vulkan instance extensions:");
    for (const auto &ext : mConfig.InstanceRequiredExtensions)
    {
        LogTrace(" - {}", ext);
    }
    LogTrace("Vulkan instance layers:");
    for (const auto &layer : mConfig.InstanceRequiredLayers)
    {
        LogTrace(" - {}", layer);
    }
    LogDebug("Vulkan instance created successfully");
}

void VulkanContext::PickPhysicalDevice()
{
    auto PhysicalDevices = Instance->enumeratePhysicalDevices();
    if (PhysicalDevices.empty())
    {
        LogError("No physical devices found");
        throw std::runtime_error("No physical devices found");
    }
    auto compare = [](const vk::PhysicalDevice &a, const vk::PhysicalDevice &b) {
        // 计算a的分数
        auto aScore = (a.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu) ? 1000 : 0;
        aScore += a.getProperties().limits.maxImageDimension2D;

        // 计算b的分数
        auto bScore = (b.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu) ? 1000 : 0;
        bScore += b.getProperties().limits.maxImageDimension2D;

        return aScore > bScore; // 降序排列
    };
    auto bestDevice = std::max_element(PhysicalDevices.begin(), PhysicalDevices.end(), compare);
    if (bestDevice == PhysicalDevices.end())
    {
        LogError("No suitable physical device found");
        throw std::runtime_error("No suitable physical device found");
    }
    PhysicalDevice = *bestDevice;
    LogDebug("Selected physical device: {}", std::string(PhysicalDevice.getProperties().deviceName.data()));
}
void VulkanContext::QueryQueueFamilyIndicates()
{
    auto queueFamilyProperties = PhysicalDevice.getQueueFamilyProperties();
    for (uint32_t i = 0; i < queueFamilyProperties.size(); i++)
    {
        auto &queueFamily = queueFamilyProperties[i];
        auto queueCount = queueFamily.queueCount;
        if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
        {
            QueueFamilyIndicates.graphicsFamily = i;
            QueueFamilyIndicates.graphicsFamilyCount = queueCount;
        }
        if (Surface && PhysicalDevice.getSurfaceSupportKHR(i, Surface.get()))
        {
            QueueFamilyIndicates.presentFamily = i;
            QueueFamilyIndicates.presentFamilyCount = queueCount;
        }
        if (queueFamily.queueFlags & vk::QueueFlagBits::eTransfer)
        {
            QueueFamilyIndicates.transferFamily = i;
            QueueFamilyIndicates.transferFamilyCount = queueCount;
        }
        if (QueueFamilyIndicates.graphicsFamily.has_value() && QueueFamilyIndicates.transferFamily.has_value())
        {
            break;
        }
    }
    LogTrace("Queue Family Indications:");
    if (QueueFamilyIndicates.graphicsFamily.has_value())
    {
        LogTrace(" - Graphics Family: {}, Count: {}", QueueFamilyIndicates.graphicsFamily.value(),
                 QueueFamilyIndicates.graphicsFamilyCount.value());
    }
    if (QueueFamilyIndicates.presentFamily.has_value())
    {
        LogTrace(" - Present Family: {}, Count: {}", QueueFamilyIndicates.presentFamily.value(),
                 QueueFamilyIndicates.presentFamilyCount.value());
    }
    if (QueueFamilyIndicates.transferFamily.has_value())
    {
        LogTrace(" - Transfer Family: {}, Count: {}", QueueFamilyIndicates.transferFamily.value(),
                 QueueFamilyIndicates.transferFamilyCount.value());
    }
}
void VulkanContext::CreateLogicalDevice()
{
    vk::DeviceCreateInfo deviceCreateInfo;
    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;

    std::set<uint32_t> uniqueQueueFamilies = {QueueFamilyIndicates.graphicsFamily.value()};

    if (QueueFamilyIndicates.transferFamily.has_value() &&
        QueueFamilyIndicates.transferFamily.value() != QueueFamilyIndicates.graphicsFamily.value())
    {
        uniqueQueueFamilies.insert(QueueFamilyIndicates.transferFamily.value());
    }

    if (QueueFamilyIndicates.presentFamily.has_value() &&
        QueueFamilyIndicates.presentFamily.value() != QueueFamilyIndicates.graphicsFamily.value())
    {
        uniqueQueueFamilies.insert(QueueFamilyIndicates.presentFamily.value());
    }

    const float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        vk::DeviceQueueCreateInfo queueCreateInfo;
        queueCreateInfo.setQueueFamilyIndex(queueFamily).setQueueCount(1).setPQueuePriorities(&queuePriority);
        queueCreateInfos.push_back(queueCreateInfo);
    }
    // extension
    auto availableExtensions = PhysicalDevice.enumerateDeviceExtensionProperties();
    for (auto &ext : availableExtensions)
    {
        LogTrace("Available Vulkan device extension: {}", ext.extensionName.data());
    }
    std::vector<const char *> extensions = {"VK_KHR_maintenance1"};
    mConfig.DeviceRequiredExtensions.insert_range(mConfig.DeviceRequiredExtensions.end(), extensions);
    deviceCreateInfo.setQueueCreateInfos(queueCreateInfos)
        .setPEnabledExtensionNames(mConfig.DeviceRequiredExtensions)
        .setPEnabledLayerNames(mConfig.DeviceRequiredLayers)
        .setPEnabledFeatures(nullptr);
    Device = PhysicalDevice.createDeviceUnique(deviceCreateInfo);
    if (!Device)
    {
        LogError("Failed to create Vulkan logical device");
        throw std::runtime_error("Failed to create Vulkan logical device");
    }
    LogDebug("Vulkan logical device created successfully");
}
void VulkanContext::InitSurface(const vk::SurfaceKHR &surface)
{
    Surface = vk::UniqueSurfaceKHR(surface, Instance.get());
    if (!Surface)
    {
        LogError("Failed to create Vulkan surface");
        throw std::runtime_error("Failed to create Vulkan surface");
    }
    LogDebug("Vulkan surface set successfully");
}
void VulkanContext::GetQueues()
{
    if (QueueFamilyIndicates.graphicsFamily.has_value())
    {
        auto graphicQueueIndex = QueueFamilyIndicates.graphicsFamily.value();
        GraphicsQueue = Device->getQueue(graphicQueueIndex, 0);
        if (!GraphicsQueue)
        {
            LogError("Failed to get graphics queue from Vulkan device");
            throw std::runtime_error("Failed to get graphics queue from Vulkan device");
        }
        LogDebug("Graphics queue obtained successfully");
    }
    if (QueueFamilyIndicates.presentFamily.has_value())
    {
        auto presentQueueIndex = QueueFamilyIndicates.presentFamily.value();
        PresentQueue = Device->getQueue(presentQueueIndex, 0);
        if (!PresentQueue)
        {
            LogError("Failed to get present queue from Vulkan device");
            throw std::runtime_error("Failed to get present queue from Vulkan device");
        }
        LogDebug("Present queue obtained successfully");
    }
    if (QueueFamilyIndicates.transferFamily.has_value())
    {
        auto transferQueueIndex = QueueFamilyIndicates.transferFamily.value();
        TransferQueue = Device->getQueue(transferQueueIndex, 0);
        if (!TransferQueue)
        {
            LogError("Failed to get transfer queue from Vulkan device");
            throw std::runtime_error("Failed to get transfer queue from Vulkan device");
        }
        LogDebug("Transfer queue obtained successfully");
    }
}

void VulkanContext::CreateCommandPools()
{
    if (QueueFamilyIndicates.graphicsFamily.has_value())
    {
        vk::CommandPoolCreateInfo commandPoolCreateInfo;
        commandPoolCreateInfo.setQueueFamilyIndex(QueueFamilyIndicates.graphicsFamily.value())
            .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
        GraphicsCommandPool = Device->createCommandPoolUnique(commandPoolCreateInfo);
        if (!GraphicsCommandPool)
        {
            LogError("Failed to create graphics command pool");
            throw std::runtime_error("Failed to create graphics command pool");
        }
        LogDebug("Graphics command pool created successfully");
    }

    if (QueueFamilyIndicates.transferFamily.has_value())
    {
        vk::CommandPoolCreateInfo commandPoolCreateInfo;
        commandPoolCreateInfo.setQueueFamilyIndex(QueueFamilyIndicates.transferFamily.value())
            .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
        TransferCommandPool = Device->createCommandPoolUnique(commandPoolCreateInfo);
        if (!TransferCommandPool)
        {
            LogError("Failed to create transfer command pool");
            throw std::runtime_error("Failed to create transfer command pool");
        }
        LogDebug("Transfer command pool created successfully");
    }

    if (QueueFamilyIndicates.presentFamily.has_value())
    {
        vk::CommandPoolCreateInfo commandPoolCreateInfo;
        commandPoolCreateInfo.setQueueFamilyIndex(QueueFamilyIndicates.presentFamily.value())
            .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
        PresentCommandPool = Device->createCommandPoolUnique(commandPoolCreateInfo);
        if (!PresentCommandPool)
        {
            LogError("Failed to create present command pool");
            throw std::runtime_error("Failed to create present command pool");
        }
        LogDebug("Present command pool created successfully");
    }
}
void VulkanContext::CreateVMA()
{
    VmaAllocatorCreateInfo allocatorCreateInfo{};
    allocatorCreateInfo.device = Device.get();
    allocatorCreateInfo.physicalDevice = PhysicalDevice;
    allocatorCreateInfo.instance = Instance.get();
    allocatorCreateInfo.flags =
        VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT | VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
    auto variant = vk::apiVersionVariant(Version);
    auto major = vk::apiVersionMajor(Version);
    auto minor = vk::apiVersionMinor(Version);
    auto patch = vk::apiVersionPatch(Version);
    allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_3;
    // allocatorCreateInfo.pVulkanFunctions = &vulkanFunctions;
    vmaCreateAllocator(&allocatorCreateInfo, &VmaAllocator);
    LogDebug("VMA Allocator created successfully");
}
void VulkanContext::QuerySurfaceInfo()
{
    auto formats = PhysicalDevice.getSurfaceFormatsKHR(Surface.get());
    auto presentModes = PhysicalDevice.getSurfacePresentModesKHR(Surface.get());
    auto capabilities = PhysicalDevice.getSurfaceCapabilitiesKHR(Surface.get());
    std::vector<vk::SurfaceFormatKHR> candidatesFormats = {
        {vk::Format::eR32G32B32A32Sfloat, vk::ColorSpaceKHR::eSrgbNonlinear},
        {vk::Format::eR16G16B16A16Sfloat, vk::ColorSpaceKHR::eSrgbNonlinear},
        {vk::Format::eR8G8B8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear},
        {vk::Format::eB8G8R8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear},
        {vk::Format::eR8G8B8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear},
        {vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear},
    };
    std::vector<vk::PresentModeKHR> candidatesPresentModes = {
        vk::PresentModeKHR::eMailbox,
        vk::PresentModeKHR::eFifo,
    };
    bool formatFound = false;
    bool presentModeFound = false;
    SurfaceInfo.format = formats[0];
    for (auto &format : candidatesFormats)
    {
        for (auto &supportFormat : formats)
        {
            if (format.format == supportFormat.format && format.colorSpace == supportFormat.colorSpace)
            {
                SurfaceInfo.format = format;
                formatFound = true;
                break;
            }
        }
        if (formatFound)
            break;
    }
    SurfaceInfo.presentMode = presentModes[0];
    for (auto &presentMode : candidatesPresentModes)
    {
        for (auto &supportPresentMode : presentModes)
        {
            if (presentMode == supportPresentMode)
            {
                SurfaceInfo.presentMode = presentMode;
                presentModeFound = true;
                break;
            }
        }
        if (presentModeFound)
            break;
    }
    SurfaceInfo.extent = capabilities.currentExtent;
    SurfaceInfo.imageCount = std::clamp(2u, capabilities.minImageCount, capabilities.maxImageCount);
    SurfaceInfo.imageArrayLayer = std::clamp(1u, 1u, capabilities.maxImageArrayLayers);
    // log
    LogTrace("Current Surface Info:");
    LogTrace("Support Image Count: {}~{}", capabilities.minImageCount, capabilities.maxImageCount);
    LogTrace("Support Array Layer: 1~{}", capabilities.maxImageArrayLayers);
    LogTrace("Support Transforms: {}", vk::to_string(capabilities.supportedTransforms));
    LogTrace("Support Usage Flags: {}", vk::to_string(capabilities.supportedUsageFlags));
    LogTrace("Support CompositeAlpha: {}", vk::to_string(capabilities.supportedCompositeAlpha));
    LogTrace("Support Extent: {}x{}~{}x{}", capabilities.minImageExtent.width, capabilities.minImageExtent.height,
             capabilities.maxImageExtent.width, capabilities.maxImageExtent.height);
    for (auto &supportFormat : formats)
    {
        LogTrace("Support Format: {}", vk::to_string(supportFormat.format));
    }
    for (auto &supportPresentMode : presentModes)
    {
        LogTrace("Support Present Mode: {}", vk::to_string(supportPresentMode));
    }
    LogTrace("Current Format: {}", vk::to_string(SurfaceInfo.format.format));
    LogTrace("Current Color Space: {}", vk::to_string(SurfaceInfo.format.colorSpace));
    LogTrace("Current Present Mode: {}", vk::to_string(SurfaceInfo.presentMode));
    LogTrace("Current Extent: {}x{}", capabilities.currentExtent.width, capabilities.currentExtent.height);
    LogTrace("Current Image Count: {}", SurfaceInfo.imageCount);
    LogTrace("Current Image Array Layer: {}", SurfaceInfo.imageArrayLayer);
}
void VulkanContext::CreateSwapchain(vk::SwapchainKHR oldSwapchain)
{
    vk::SwapchainCreateInfoKHR swapchainCreateInfo;
    auto queueFamilyIndicates = QueueFamilyIndicates;
    swapchainCreateInfo.setSurface(Surface.get())
        .setClipped(true)
        .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
        .setImageArrayLayers(SurfaceInfo.imageArrayLayer)
        .setMinImageCount(SurfaceInfo.imageCount)
        .setImageColorSpace(SurfaceInfo.format.colorSpace)
        .setImageExtent(SurfaceInfo.extent)
        .setImageFormat(SurfaceInfo.format.format)
        .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc |
                       vk::ImageUsageFlagBits::eTransferDst)
        .setPresentMode(SurfaceInfo.presentMode)
        .setPreTransform(vk::SurfaceTransformFlagBitsKHR::eIdentity)
        .setOldSwapchain(oldSwapchain);

    if (queueFamilyIndicates.graphicsFamily == queueFamilyIndicates.presentFamily)
    {
        swapchainCreateInfo.setImageSharingMode(vk::SharingMode::eExclusive)
            .setQueueFamilyIndices({queueFamilyIndicates.graphicsFamily.value()});
    }
    else
    {
        std::array<uint32_t, 2> queueFamilyIndicesArray = {queueFamilyIndicates.graphicsFamily.value(),
                                                           queueFamilyIndicates.presentFamily.value()};
        swapchainCreateInfo.setImageSharingMode(vk::SharingMode::eConcurrent)
            .setQueueFamilyIndices(queueFamilyIndicesArray);
    }

    mSwapchain = Device->createSwapchainKHRUnique(swapchainCreateInfo);
    if (!mSwapchain)
    {
        LogError("Failed to create swapchain");
        throw std::runtime_error("Failed to create swapchain");
    }
    LogDebug("Swapchain created successfully");
}
void VulkanContext::RecreateSwapchain()
{
    Device->waitIdle();
    QuerySurfaceInfo();
    auto oldSwapchain = std::move(mSwapchain);
    CreateSwapchain(oldSwapchain.get());
    mSwapchainImageViews.clear();
    mSwapchainImages.clear();
    CreateSwapchainImages();
    CreateSwapchainImageViews();
    LogDebug("Swapchain Recreated");
}
void VulkanContext::CreateSwapchainImages()
{
    auto swapchainImages = Device->getSwapchainImagesKHR(mSwapchain.get());
    for (auto &image : swapchainImages)
    {
        mSwapchainImages.push_back(image);
    }
    LogDebug("Swapchain Images Created");
    LogTrace("Swapchain Image Count: {}", mSwapchainImages.size());
}
void VulkanContext::CreateSwapchainImageViews()
{
    for (auto image : mSwapchainImages)
    {
        vk::ImageViewCreateInfo imageViewCreateInfo;
        imageViewCreateInfo.setImage(image)
            .setViewType(vk::ImageViewType::e2D)
            .setFormat(SurfaceInfo.format.format)
            .setComponents(vk::ComponentMapping{})
            .setSubresourceRange(vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
        auto imageView = Device->createImageViewUnique(imageViewCreateInfo);
        if (!imageView)
        {
            LogError("Failed to create image view for swapchain image");
            throw std::runtime_error("Failed to create image view for swapchain image");
        }
        mSwapchainImageViews.push_back(std::move(imageView));
    }
    LogTrace("Swapchain Image Count: {}", mSwapchainImageViews.size());
    LogDebug("Swapchain Image Views Created");
}
void VulkanContext::CreateDescriptorPool()
{
    auto maxDescriptorSize = 1000;
    std::vector<std::pair<vk::DescriptorType, float>> proportion = {
        {vk::DescriptorType::eSampler, 0.5f},
        {vk::DescriptorType::eCombinedImageSampler, 4.0f},
        {vk::DescriptorType::eSampledImage, 4.0f},
        {vk::DescriptorType::eStorageImage, 1.0f},
        {vk::DescriptorType::eUniformBuffer, 2.0f},
        {vk::DescriptorType::eStorageBuffer, 2.0f},
        {vk::DescriptorType::eUniformBufferDynamic, 1.0f},
        {vk::DescriptorType::eStorageBufferDynamic, 1.0f},
    };

    std::vector<vk::DescriptorPoolSize> descriptorPoolSize;
    descriptorPoolSize.reserve(proportion.size());
    for (auto &proportion : proportion)
    {
        descriptorPoolSize.emplace_back(proportion.first, static_cast<uint32_t>(proportion.second * maxDescriptorSize));
    }
    vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo;
    descriptorPoolCreateInfo
        .setPoolSizes(descriptorPoolSize) // 设置池中各类描述符的数量
        .setMaxSets(maxDescriptorSize)    // 设置池最多可分配的 Descriptor Set 数量
        .setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet |
                  vk::DescriptorPoolCreateFlagBits::eUpdateAfterBind); // 设置池的标志位
    DescriptorPool = Device->createDescriptorPoolUnique(descriptorPoolCreateInfo);
    if (!DescriptorPool)
    {
        LogError("Failed to create descriptor pool");
        throw std::runtime_error("Failed to create descriptor pool");
    }
    LogDebug("Descriptor Pool created successfully");
}
} // namespace MEngine