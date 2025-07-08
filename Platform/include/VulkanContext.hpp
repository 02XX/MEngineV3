#pragma once
#include "VMA.hpp"
#include <memory>
#include <optional>
#include <vulkan/vulkan.hpp>

namespace MEngine
{
struct VulkanContextConfig
{
    std::vector<const char *> InstanceRequiredExtensions;
    std::vector<const char *> InstanceRequiredLayers;
    std::vector<const char *> DeviceRequiredExtensions;
    std::vector<const char *> DeviceRequiredLayers;
};
class VulkanContext
{
  private:
    VulkanContextConfig mConfig;
    // properties
    vk::UniqueInstance Instance;
    vk::PhysicalDevice PhysicalDevice;
    struct QueueFamilyIndicates
    {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> graphicsFamilyCount;
        std::optional<uint32_t> presentFamily;
        std::optional<uint32_t> presentFamilyCount;
        std::optional<uint32_t> transferFamily;
        std::optional<uint32_t> transferFamilyCount;
    } QueueFamilyIndicates;
    struct SurfaceInfo
    {
        vk::SurfaceFormatKHR format;
        vk::Extent2D extent;
        vk::PresentModeKHR presentMode;
        uint32_t imageCount;
        uint32_t imageArrayLayer;
    } SurfaceInfo;
    vk::UniqueDevice Device;
    vk::UniqueSurfaceKHR Surface;
    vk::UniqueCommandPool GraphicsCommandPool;
    vk::UniqueCommandPool TransferCommandPool;
    vk::UniqueCommandPool PresentCommandPool;
    vk::Queue GraphicsQueue;
    vk::Queue TransferQueue;
    vk::Queue PresentQueue;
    uint32_t Version = 0;
    vk::UniqueDescriptorPool DescriptorPool;

    // VMA
    VmaAllocator VmaAllocator;

    // Swapchain
    vk::UniqueSwapchainKHR mSwapchain;
    std::vector<vk::Image> mSwapchainImages;
    std::vector<vk::UniqueImageView> mSwapchainImageViews;

  public:
    inline const VulkanContextConfig &GetConfig() const
    {
        return mConfig;
    }
    inline const vk::Instance &GetInstance() const
    {
        return *Instance;
    }
    inline const vk::PhysicalDevice &GetPhysicalDevice() const
    {
        return PhysicalDevice;
    }
    inline const vk::Device &GetDevice() const
    {
        return *Device;
    }
    inline const vk::SurfaceKHR &GetSurface() const
    {
        return *Surface;
    }
    inline const vk::Queue &GetGraphicsQueue() const
    {
        return GraphicsQueue;
    }
    inline const vk::Queue &GetTransferQueue() const
    {
        return TransferQueue;
    }
    inline const vk::Queue &GetPresentQueue() const
    {
        return PresentQueue;
    }
    inline const vk::CommandPool &GetGraphicsCommandPool() const
    {
        return *GraphicsCommandPool;
    }
    inline const vk::CommandPool &GetTransferCommandPool() const
    {
        return *TransferCommandPool;
    }
    inline const vk::CommandPool &GetPresentCommandPool() const
    {
        return *PresentCommandPool;
    }
    inline const struct QueueFamilyIndicates &GetQueueFamilyIndicates() const
    {
        return QueueFamilyIndicates;
    }
    inline uint32_t GetVersion() const
    {
        return Version;
    }

    inline const ::VmaAllocator &GetVmaAllocator() const
    {
        return VmaAllocator;
    }
    inline const vk::SwapchainKHR &GetSwapchain() const
    {
        return *mSwapchain;
    }
    inline const std::vector<vk::Image> &GetSwapchainImages() const
    {
        return mSwapchainImages;
    }
    inline const std::vector<vk::UniqueImageView> &GetSwapchainImageViews() const
    {
        return mSwapchainImageViews;
    }
    inline const struct SurfaceInfo &GetSurfaceInfo() const
    {
        return SurfaceInfo;
    }
    inline const vk::DescriptorPool GetDescriptorPool() const
    {
        return DescriptorPool.get();
    }
    void RecreateSwapchain();

  private:
    void CreateInstance();
    void PickPhysicalDevice();
    void QueryQueueFamilyIndicates();
    void CreateLogicalDevice();
    void GetQueues();
    void CreateCommandPools();
    void CreateVMA();
    void QuerySurfaceInfo();
    void CreateSwapchain(vk::SwapchainKHR oldSwapchain = nullptr);
    void CreateSwapchainImages();
    void CreateSwapchainImageViews();
    void CreateDescriptorPool();

  public:
    VulkanContext();
    ~VulkanContext();
    void InitContext(const VulkanContextConfig &config);
    void Init();

    void InitSurface(const vk::SurfaceKHR &surface);
    void Destroy();
};

} // namespace MEngine