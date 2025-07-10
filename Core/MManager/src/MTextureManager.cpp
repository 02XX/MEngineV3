#include "MTextureManager.hpp"
#include "ImageUtil.hpp"
#include "Logger.hpp"
#include "MTexture.hpp"
#include "VulkanContext.hpp"
#include <imgui_impl_vulkan.h>
#include <memory>
#include <vulkan/vulkan_enums.hpp>

namespace MEngine::Core::Manager
{
vk::ImageType MTextureManager::TextureTypeToImageType(vk::ImageViewType type)
{
    switch (type)
    {
    case vk::ImageViewType::e1D:
        return vk::ImageType::e1D;
    case vk::ImageViewType::e2D:
        return vk::ImageType::e2D;
    case vk::ImageViewType::e3D:
        return vk::ImageType::e3D;
    case vk::ImageViewType::eCube:
        return vk::ImageType::e2D;
    case vk::ImageViewType::e1DArray:
        return vk::ImageType::e1D;
    case vk::ImageViewType::e2DArray:
        return vk::ImageType::e2D;
    case vk::ImageViewType::eCubeArray:
        return vk::ImageType::e2D;
    default:
        return vk::ImageType::e2D;
    }
}
vk::ImageUsageFlags MTextureManager::PickImageUsage(const MTextureSetting &setting)
{
    vk::ImageUsageFlags ret = vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst;

    if (setting.isShaderResource)
        ret |= vk::ImageUsageFlagBits::eSampled;

    if (setting.isRenderTarget)
    {
        switch (setting.format)
        {
        case vk::Format::eD32SfloatS8Uint:
        case vk::Format::eD24UnormS8Uint:
        case vk::Format::eD16UnormS8Uint:
        case vk::Format::eD32Sfloat:
        case vk::Format::eD16Unorm:
            ret |= vk::ImageUsageFlagBits::eDepthStencilAttachment;
            break;
        default:
            ret |= vk::ImageUsageFlagBits::eColorAttachment;
            break;
        }
    }

    if (setting.isUAV)
        ret |= vk::ImageUsageFlagBits::eStorage;

    if (setting.isShadingRateSurface)
        ret |= vk::ImageUsageFlagBits::eFragmentShadingRateAttachmentKHR;

    return ret;
}
vk::ImageCreateFlags MTextureManager::PickImageFlags(const MTextureSetting &setting)
{
    vk::ImageCreateFlags flags = vk::ImageCreateFlags(0);

    if (setting.ImageType == vk::ImageViewType::eCube || setting.ImageType == vk::ImageViewType::eCubeArray)
        flags |= vk::ImageCreateFlagBits::eCubeCompatible;
    if (setting.isTypeless)
        flags |= vk::ImageCreateFlagBits::eMutableFormat | vk::ImageCreateFlagBits::eExtendedUsage;
    if (setting.isTiled)
        flags |= vk::ImageCreateFlagBits::eSparseBinding | vk::ImageCreateFlagBits::eSparseResidency;
    return flags;
}
vk::ImageAspectFlags MTextureManager::GuessImageAspectFlags(vk::Format format)
{
    switch (format)
    {
    case vk::Format::eD16Unorm:
    case vk::Format::eX8D24UnormPack32:
    case vk::Format::eD32Sfloat:
        return vk::ImageAspectFlagBits::eDepth;

    case vk::Format::eS8Uint:
        return vk::ImageAspectFlagBits::eStencil;

    case vk::Format::eD16UnormS8Uint:
    case vk::Format::eD24UnormS8Uint:
    case vk::Format::eD32SfloatS8Uint:
        return vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;

    default:
        return vk::ImageAspectFlagBits::eColor;
    }
}
MTextureManager::MTextureManager(std::shared_ptr<VulkanContext> vulkanContext,
                                 std::shared_ptr<IUUIDGenerator> uuidGenerator)
    : MManager(vulkanContext, uuidGenerator)
{
    // 创建命令缓冲区
    auto device = vulkanContext->GetDevice();
    vk::CommandBufferAllocateInfo commandBufferAllocateInfo{};
    commandBufferAllocateInfo.setCommandPool(vulkanContext->GetTransferCommandPool())
        .setLevel(vk::CommandBufferLevel::ePrimary)
        .setCommandBufferCount(1);
    mCommandBuffer = std::move(device.allocateCommandBuffersUnique(commandBufferAllocateInfo)[0]);
    if (!mCommandBuffer)
    {
        LogError("Failed to create command buffer for MTextureManager");
        throw std::runtime_error("Failed to create command buffer for MTextureManager");
    }
    // 创建同步对象
    vk::FenceCreateInfo fenceCreateInfo{};
    mFence = device.createFenceUnique(fenceCreateInfo);
    if (!mFence)
    {
        LogError("Failed to create fence for MTextureManager");
        throw std::runtime_error("Failed to create fence for MTextureManager");
    }
    CreateDefault();
}
std::shared_ptr<MTexture> MTextureManager::Create(const MTextureSetting &setting, const std::string &name)
{
    auto texture = std::make_shared<MTexture>(mUUIDGenerator->Create(), name, mVulkanContext, setting);
    mAssets[texture->GetID()] = texture;
    return texture;
}
void MTextureManager::Update(std::shared_ptr<MTexture> texture)
{
}
void MTextureManager::CreateVulkanResources(std::shared_ptr<MTexture> texture)
{
    if (texture->mImage)
    {
        VkImage image = texture->mImage;
        vmaDestroyImage(mVulkanContext->GetVmaAllocator(), image, texture->mAllocation);
    }
    vk::ImageCreateInfo imageCreateInfo{};
    imageCreateInfo.setImageType(TextureTypeToImageType(texture->mSetting.ImageType))
        .setExtent({texture->mSetting.width, texture->mSetting.height, 1})
        .setMipLevels(texture->mSetting.mipmapLevels)
        .setArrayLayers(texture->mSetting.arrayLayers)
        .setFormat(texture->mSetting.format)
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setUsage(PickImageUsage(texture->mSetting))
        .setSharingMode(vk::SharingMode::eExclusive)
        .setSamples(texture->mSetting.sampleCount)
        .setFlags(PickImageFlags(texture->mSetting));
    if (PickImageUsage(texture->mSetting) & vk::ImageUsageFlagBits::eDepthStencilAttachment)
    {
        texture->mSetting.isDepthStencil = true;
    }
    VmaAllocationCreateInfo allocationCreateInfo = {};
    allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    if (vmaCreateImage(mVulkanContext->GetVmaAllocator(), &static_cast<VkImageCreateInfo &>(imageCreateInfo),
                       &allocationCreateInfo, reinterpret_cast<VkImage *>(&texture->mImage), &texture->mAllocation,
                       &texture->mAllocationInfo) != VK_SUCCESS)
    {
        LogError("Failed to create Vulkan image");
        throw std::runtime_error("Failed to create Vulkan image");
    }
    vk::SamplerCreateInfo samplerCreateInfo{};
    samplerCreateInfo.setAddressModeU(texture->mSetting.addressModeU)
        .setAddressModeV(texture->mSetting.addressModeV)
        .setAddressModeW(texture->mSetting.addressModeW)
        .setMinFilter(texture->mSetting.minFilter)
        .setMagFilter(texture->mSetting.magFilter)
        .setMipmapMode(texture->mSetting.mipmapMode)
        .setMipLodBias(texture->mSetting.mipLodBias)
        .setMinLod(texture->mSetting.minLod)
        .setMaxLod(texture->mSetting.maxLod)
        .setBorderColor(texture->mSetting.borderColor)
        .setUnnormalizedCoordinates(texture->mSetting.unnormalizedCoordinates)
        .setCompareEnable(texture->mSetting.compareEnable)
        .setCompareOp(texture->mSetting.compareOp)
        .setAnisotropyEnable(texture->mSetting.anisotropyEnable)
        .setMaxAnisotropy(texture->mSetting.maxAnisotropy);
    texture->mSampler = mVulkanContext->GetDevice().createSamplerUnique(samplerCreateInfo);
    if (!texture->mSampler)
    {
        LogError("Failed to create texture sampler");
    }

    vk::ImageViewCreateInfo imageViewCreateInfo{};
    imageViewCreateInfo.setImage(texture->mImage)
        .setViewType(texture->mSetting.ImageType)
        .setFormat(texture->mSetting.format)
        .setSubresourceRange(vk::ImageSubresourceRange()
                                 .setAspectMask(GuessImageAspectFlags(texture->mSetting.format))
                                 .setBaseMipLevel(0)
                                 .setLevelCount(texture->mSetting.mipmapLevels)
                                 .setBaseArrayLayer(0)
                                 .setLayerCount(texture->mSetting.arrayLayers));
    texture->mImageView = mVulkanContext->GetDevice().createImageViewUnique(imageViewCreateInfo);
    if (!texture->mImageView)
    {
        LogError("Failed to create texture image view");
    }
    // 创建缩略图描述集
    if (!texture->mSetting.isDepthStencil)
    {
        texture->mThumbnailDescriptorSet =
            ImGui_ImplVulkan_AddTexture(texture->mSampler.get(), texture->mImageView.get(),
                                        static_cast<VkImageLayout>(vk::ImageLayout::eShaderReadOnlyOptimal));
    }
}
void MTextureManager::Write(std::shared_ptr<MTexture> texture, const std::vector<uint8_t> &data,
                            const TextureSize &size)
{
    mCommandBuffer->reset();
    mVulkanContext->GetDevice().resetFences(mFence.get());
    vk::Buffer stagingBuffer;
    vk::BufferCreateInfo stagingBufferCreateInfo{};
    stagingBufferCreateInfo.setSize(size.width * size.height * size.channels)
        .setUsage(vk::BufferUsageFlagBits::eTransferSrc)
        .setSharingMode(vk::SharingMode::eExclusive);

    VmaAllocationCreateInfo stagingAllocationCreateInfo{};
    stagingAllocationCreateInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
    stagingAllocationCreateInfo.flags =
        VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
    VmaAllocation stagingAllocation;
    VmaAllocationInfo stagingAllocationInfo;
    if (vmaCreateBuffer(mVulkanContext->GetVmaAllocator(), &static_cast<VkBufferCreateInfo &>(stagingBufferCreateInfo),
                        &stagingAllocationCreateInfo, reinterpret_cast<VkBuffer *>(&stagingBuffer), &stagingAllocation,
                        &stagingAllocationInfo) != VK_SUCCESS)
    {
        LogError("Failed to create staging buffer");
    }
    memcpy(stagingAllocationInfo.pMappedData, data.data(), size.width * size.height * size.channels);
    mCommandBuffer->begin(vk::CommandBufferBeginInfo{});
    {
        // 转换图像布局UNDEFINED → TRANSFER_DST
        vk::ImageMemoryBarrier imageBarrier{};
        imageBarrier.setImage(texture->mImage)
            .setOldLayout(vk::ImageLayout::eUndefined)
            .setNewLayout(vk::ImageLayout::eTransferDstOptimal)
            .setSrcQueueFamilyIndex(mVulkanContext->GetQueueFamilyIndicates().graphicsFamily.value())
            .setDstQueueFamilyIndex(mVulkanContext->GetQueueFamilyIndicates().graphicsFamily.value())
            .setSubresourceRange(vk::ImageSubresourceRange()
                                     .setAspectMask(GuessImageAspectFlags(texture->mSetting.format))
                                     .setBaseMipLevel(0)
                                     .setLevelCount(texture->mSetting.mipmapLevels)
                                     .setBaseArrayLayer(0)
                                     .setLayerCount(texture->mSetting.arrayLayers));
        mCommandBuffer->pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer, {},
                                        {}, {}, {imageBarrier});
        // 复制数据到图像
        vk::BufferImageCopy bufferImageCopy{};
        bufferImageCopy.setBufferOffset(0)
            .setBufferRowLength(0)
            .setBufferImageHeight(0)
            .setImageSubresource(vk::ImageSubresourceLayers()
                                     .setAspectMask(GuessImageAspectFlags(texture->mSetting.format))
                                     .setMipLevel(0)
                                     .setBaseArrayLayer(0)
                                     .setLayerCount(texture->mSetting.arrayLayers))
            .setImageOffset({0, 0, 0})
            .setImageExtent({size.width, size.height, 1});
        mCommandBuffer->copyBufferToImage(stagingBuffer, texture->mImage, vk::ImageLayout::eTransferDstOptimal,
                                          {bufferImageCopy});
        // 转换图像布局：TRANSFER_DST → SHADER_READ
        imageBarrier.setOldLayout(vk::ImageLayout::eTransferDstOptimal)
            .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
            .setSrcQueueFamilyIndex(mVulkanContext->GetQueueFamilyIndicates().graphicsFamily.value())
            .setDstQueueFamilyIndex(mVulkanContext->GetQueueFamilyIndicates().graphicsFamily.value());
        mCommandBuffer->pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
                                        vk::PipelineStageFlagBits::eFragmentShader, {}, {}, {}, {imageBarrier});
    }
    mCommandBuffer->end();
    // 提交命令缓冲区
    vk::SubmitInfo submitInfo{};
    submitInfo.setCommandBuffers(mCommandBuffer.get());
    mVulkanContext->GetTransferQueue().submit(submitInfo, mFence.get());
    // 等待提交完成
    auto result = mVulkanContext->GetDevice().waitForFences(mFence.get(), vk::True, 100000000); // 10秒超时
    if (result != vk::Result::eSuccess)
    {
        LogError("Failed to wait for fence: {}", vk::to_string(result));
        throw std::runtime_error("Failed to wait for fence");
    }
    // 清理临时资源
    vmaDestroyBuffer(mVulkanContext->GetVmaAllocator(), stagingBuffer, stagingAllocation);
}
void MTextureManager::CreateDefault()
{
    mDefaultTextures[DefaultTextureType::White] = CreateWhiteTexture();
    mDefaultTextures[DefaultTextureType::Black] = CreateBlackTexture();
    mDefaultTextures[DefaultTextureType::Normal] = CreateNormalTexture();
    mDefaultTextures[DefaultTextureType::Emissive] = CreateEmissiveTexture();
    mDefaultTextures[DefaultTextureType::Albedo] = CreateAlbedoTexture();
    mDefaultTextures[DefaultTextureType::ARM] = CreateARMTexture();

    auto noColorTextureSetting = MTextureSetting();
    noColorTextureSetting.width = 1;
    noColorTextureSetting.height = 1;
    noColorTextureSetting.format = vk::Format::eR8G8B8A8Unorm;
    noColorTextureSetting.isShaderResource = true;
    auto noColorTexture = Create(noColorTextureSetting, "No Texture");
    Remove(noColorTexture->GetID());
    noColorTexture->SetID(UUID{});
    mAssets[noColorTexture->GetID()] = noColorTexture;
    auto magentaPixel = std::vector<uint8_t>{255, 0, 255, 255}; // Magenta color
    CreateVulkanResources(noColorTexture);
    Write(noColorTexture, magentaPixel, TextureSize{noColorTextureSetting.width, noColorTextureSetting.height, 4});
}
std::shared_ptr<MTexture> MTextureManager::CreateWhiteTexture()
{
    auto whiteTextureSetting = MTextureSetting();
    auto whiteTexture = Create(whiteTextureSetting, "Default White Texture");
    CreateVulkanResources(whiteTexture);
    Write(whiteTexture, GetWhiteData(), TextureSize{whiteTextureSetting.width, whiteTextureSetting.height, 4});
    return whiteTexture;
}
std::shared_ptr<MTexture> MTextureManager::CreateBlackTexture()
{
    auto blackTextureSetting = MTextureSetting();
    auto blackTexture = Create(blackTextureSetting, "Default Black Texture");
    CreateVulkanResources(blackTexture);
    Write(blackTexture, GetBlackData(), TextureSize{blackTextureSetting.width, blackTextureSetting.height, 4});
    return blackTexture;
}
std::shared_ptr<MTexture> MTextureManager::CreateNormalTexture()
{
    auto normalTextureSetting = MTextureSetting();
    auto normalTexture = Create(normalTextureSetting, "Default Normal Texture");
    CreateVulkanResources(normalTexture);
    Write(normalTexture, GetNormalData(), TextureSize{normalTextureSetting.width, normalTextureSetting.height, 4});
    return normalTexture;
}
std::shared_ptr<MTexture> MTextureManager::CreateEmissiveTexture()
{
    auto emissiveTextureSetting = MTextureSetting();
    auto emissiveTexture = Create(emissiveTextureSetting, "Default Emissive Texture");
    CreateVulkanResources(emissiveTexture);
    Write(emissiveTexture, GetEmissiveData(),
          TextureSize{emissiveTextureSetting.width, emissiveTextureSetting.height, 4});
    return emissiveTexture;
}
std::shared_ptr<MTexture> MTextureManager::CreateAlbedoTexture()
{
    auto albedoTextureSetting = MTextureSetting();
    auto albedoTexture = Create(albedoTextureSetting, "Default Albedo Texture");
    CreateVulkanResources(albedoTexture);
    Write(albedoTexture, GetAlbedoData(), TextureSize{albedoTextureSetting.width, albedoTextureSetting.height, 4});
    return albedoTexture;
}
std::shared_ptr<MTexture> MTextureManager::CreateARMTexture()
{
    auto armTextureSetting = MTextureSetting();
    auto armTexture = Create(armTextureSetting, "Default ARM Texture");
    CreateVulkanResources(armTexture);
    Write(armTexture, GetARMData(), TextureSize{armTextureSetting.width, armTextureSetting.height, 4});
    return armTexture;
}
std::shared_ptr<MTexture> MTextureManager::GetDefaultTexture(DefaultTextureType type) const
{
    if (mDefaultTextures.find(type) != mDefaultTextures.end())
    {
        return mDefaultTextures.at(type);
    }
    LogError("Default texture type {} not found", static_cast<int>(type));
    return nullptr;
}
} // namespace MEngine::Core::Manager
