#include "MTextureManager.hpp"
#include "ImageUtil.hpp"
#include "Logger.hpp"
#include "MTexture.hpp"
#include "VulkanContext.hpp"
#include <memory>

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
}
std::shared_ptr<MTexture> MTextureManager::Create(const MTextureSetting &setting)
{
    auto texture = std::shared_ptr<MTexture>(new MTexture(mUUIDGenerator->Create(), mVulkanContext, setting));
    vk::ImageCreateInfo imageCreateInfo{};
    imageCreateInfo.setImageType(TextureTypeToImageType(texture->mSetting.ImageType))
        .setExtent({setting.width, setting.height, 1})
        .setMipLevels(texture->mSetting.mipmapLevels)
        .setArrayLayers(texture->mSetting.arrayLayers)
        .setFormat(texture->mSetting.format)
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setUsage(PickImageUsage(texture->mSetting))
        .setSharingMode(vk::SharingMode::eExclusive)
        .setSamples(texture->mSetting.sampleCount)
        .setFlags(PickImageFlags(texture->mSetting));

    VmaAllocationCreateInfo allocationCreateInfo = {};
    allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    if (vmaCreateImage(mVulkanContext->GetVmaAllocator(), &static_cast<VkImageCreateInfo &>(imageCreateInfo),
                       &allocationCreateInfo, reinterpret_cast<VkImage *>(&texture->mImage), &texture->mAllocation,
                       &texture->mAllocationInfo) != VK_SUCCESS)
    {
        LogError("Failed to create Vulkan image");
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
    return texture;
}
void MTextureManager::Update(std::shared_ptr<MTexture> texture)
{
    auto newTexture = Create(texture->mSetting);
    if (texture->mImage)
    {
        VkImage image = texture->mImage;
        vmaDestroyImage(mVulkanContext->GetVmaAllocator(), image, texture->mAllocation);
    }
    texture->mImage = std::move(newTexture->mImage);
    texture->mAllocation = std::move(newTexture->mAllocation);
    texture->mAllocationInfo = std::move(newTexture->mAllocationInfo);
    texture->mSampler = std::move(newTexture->mSampler);
    texture->mImageView = std::move(newTexture->mImageView);
    texture->mSetting = newTexture->mSetting;
}
void MTextureManager::Write(std::shared_ptr<MTexture> texture, const std::filesystem::path &path)
{
    stbi_set_unpremultiply_on_load(true);
    int width, height, channels;
    unsigned char *data = stbi_load(path.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);
    if (!data)
    {
        LogError("Failed to load texture image");
        return;
    }
    TextureSize size{static_cast<uint32_t>(width), static_cast<uint32_t>(height), static_cast<uint32_t>(channels)};
    Write(texture, std::vector<uint8_t>(data, data + width * height * channels), size);
    stbi_image_free(data);
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
} // namespace MEngine::Core::Manager
