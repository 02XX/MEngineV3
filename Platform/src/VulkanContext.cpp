#include "VulkanContext.hpp"
#include "Logger.hpp"
#include <set>

namespace MEngine
{

VulkanContext::VulkanContext(std::shared_ptr<VulkanContextConfig> config) : mConfig(config)
{
    if (!mConfig)
    {
        mConfig = std::make_shared<VulkanContextConfig>();
    }
    CreateInstance();
    PickPhysicalDevice();
}
void VulkanContext::Init()
{
    QueryQueueFamilyIndicates();
    CreateLogicalDevice();
    GetQueues();
    CreateCommandPools();
}

void VulkanContext::Destroy()
{
    GraphicsCommandPool.reset();
    TransferCommandPool.reset();
    PresentCommandPool.reset();
    Device.reset();
    PhysicalDevice = nullptr;
    Instance.reset();
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
    instanceCreateInfo.setFlags({})
        .setPApplicationInfo(&appInfo)
        .setPEnabledLayerNames(mConfig->InstanceRequiredLayers)
        .setPEnabledExtensionNames(mConfig->InstanceRequiredExtensions);
    Instance = vk::createInstanceUnique(instanceCreateInfo);
    if (!Instance)
    {
        LogError("Failed to create Vulkan instance");
        throw std::runtime_error("Failed to create Vulkan instance");
    }
    LogTrace("Vulkan instance created with version: {}.{}.{}.{}", variant, major, minor, patch);
    LogTrace("Vulkan instance extensions:");
    for (const auto &ext : mConfig->InstanceRequiredExtensions)
    {
        LogTrace(" - {}", ext);
    }
    LogTrace("Vulkan instance layers:");
    for (const auto &layer : mConfig->InstanceRequiredLayers)
    {
        LogTrace(" - {}", layer);
    }
    LogInfo("Vulkan instance created successfully");
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
    LogInfo("Selected physical device: {}", std::string(PhysicalDevice.getProperties().deviceName.data()));
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
    LogInfo("Queue Family Indications:");
    if (QueueFamilyIndicates.graphicsFamily.has_value())
    {
        LogInfo(" - Graphics Family: {}, Count: {}", QueueFamilyIndicates.graphicsFamily.value(),
                QueueFamilyIndicates.graphicsFamilyCount.value());
    }
    if (QueueFamilyIndicates.presentFamily.has_value())
    {
        LogInfo(" - Present Family: {}, Count: {}", QueueFamilyIndicates.presentFamily.value(),
                QueueFamilyIndicates.presentFamilyCount.value());
    }
    if (QueueFamilyIndicates.transferFamily.has_value())
    {
        LogInfo(" - Transfer Family: {}, Count: {}", QueueFamilyIndicates.transferFamily.value(),
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
    deviceCreateInfo.setQueueCreateInfos(queueCreateInfos)
        .setPEnabledExtensionNames(mConfig->DeviceRequiredExtensions)
        .setPEnabledLayerNames(mConfig->DeviceRequiredLayers)
        .setPEnabledFeatures(nullptr);
    Device = PhysicalDevice.createDeviceUnique(deviceCreateInfo);
    if (!Device)
    {
        LogError("Failed to create Vulkan logical device");
        throw std::runtime_error("Failed to create Vulkan logical device");
    }
    LogInfo("Vulkan logical device created successfully");
}
void VulkanContext::SetSurface(const vk::SurfaceKHR &surface)
{
    Surface = vk::UniqueSurfaceKHR(surface, Instance.get());
    if (!Surface)
    {
        LogError("Failed to create Vulkan surface");
        throw std::runtime_error("Failed to create Vulkan surface");
    }
    LogInfo("Vulkan surface set successfully");
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
        LogInfo("Graphics queue obtained successfully");
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
        LogInfo("Present queue obtained successfully");
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
        LogInfo("Transfer queue obtained successfully");
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
        LogInfo("Graphics command pool created successfully");
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
        LogInfo("Transfer command pool created successfully");
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
        LogInfo("Present command pool created successfully");
    }
}

} // namespace MEngine