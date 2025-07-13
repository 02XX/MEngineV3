#include "MTextureManager.hpp"
#include "IMTextureManager.hpp"
#include "ImageUtil.hpp"
#include "Logger.hpp"
#include "MTexture.hpp"
#include "VulkanContext.hpp"
#include <cstdint>
#include <imgui_impl_vulkan.h>
#include <ktx.h>
#include <ktxvulkan.h>
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
std::pair<uint32_t, uint32_t> MTextureManager::PickPixelSize(vk::Format format)
{
    // return {channels, bytesPerPixel};
    switch (format)
    {
    case vk::Format::eR8Unorm:
    case vk::Format::eR8Snorm:
    case vk::Format::eR8Uint:
    case vk::Format::eR8Sint:
    case vk::Format::eR8Srgb:
        return {1, 1};

    case vk::Format::eR8G8Unorm:
    case vk::Format::eR8G8Snorm:
    case vk::Format::eR8G8Uint:
    case vk::Format::eR8G8Sint:
    case vk::Format::eR8G8Srgb:
        return {2, 2};

    case vk::Format::eR8G8B8Unorm:
    case vk::Format::eR8G8B8Snorm:
    case vk::Format::eR8G8B8Uint:
    case vk::Format::eR8G8B8Sint:
    case vk::Format::eR8G8B8Srgb:
        return {3, 3};

    case vk::Format::eR8G8B8A8Unorm:
    case vk::Format::eR8G8B8A8Snorm:
    case vk::Format::eR8G8B8A8Uint:
    case vk::Format::eR8G8B8A8Sint:
    case vk::Format::eR8G8B8A8Srgb:
        return {4, 4};

    case vk::Format::eR16Unorm:
    case vk::Format::eR16Snorm:
    case vk::Format::eR16Uint:
    case vk::Format::eR16Sint:
    case vk::Format::eR16Sfloat:
        return {1, 2};

    case vk::Format::eR16G16Unorm:
    case vk::Format::eR16G16Snorm:
    case vk::Format::eR16G16Uint:
    case vk::Format::eR16G16Sint:
    case vk::Format::eR16G16Sfloat:
        return {2, 4};

    case vk::Format::eR16G16B16Unorm:
    case vk::Format::eR16G16B16Snorm:
    case vk::Format::eR16G16B16Uint:
    case vk::Format::eR16G16B16Sint:
    case vk::Format::eR16G16B16Sfloat:
        return {3, 6};

    case vk::Format::eR16G16B16A16Unorm:
    case vk::Format::eR16G16B16A16Snorm:
    case vk::Format::eR16G16B16A16Uint:
    case vk::Format::eR16G16B16A16Sint:
    case vk::Format::eR16G16B16A16Sfloat:
        return {4, 8};

    case vk::Format::eR32Uint:
    case vk::Format::eR32Sint:
    case vk::Format::eR32Sfloat:
        return {1, 4};

    case vk::Format::eR32G32Uint:
    case vk::Format::eR32G32Sint:
    case vk::Format::eR32G32Sfloat:
        return {2, 8};

    case vk::Format::eR32G32B32Uint:
    case vk::Format::eR32G32B32Sint:
    case vk::Format::eR32G32B32Sfloat:
        return {3, 12};

    case vk::Format::eR32G32B32A32Uint:
    case vk::Format::eR32G32B32A32Sint:
    case vk::Format::eR32G32B32A32Sfloat:
        return {4, 16};

    case vk::Format::eD16Unorm:
        return {1, 2};

    case vk::Format::eD32Sfloat:
        return {1, 4};

    case vk::Format::eD24UnormS8Uint:
        return {1, 4};

    case vk::Format::eD32SfloatS8Uint:
        return {1, 5};

    default:
        LogError("Unknown format: {}", vk::to_string(format));
        return {0, 0};
    }
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
std::shared_ptr<MTexture> MTextureManager::Create(const std::string &name, TextureSize size,
                                                  const std::vector<uint8_t> &imageData, const MTextureSetting &setting)
{
    auto texture = std::make_shared<MTexture>(mUUIDGenerator->Create(), name, mVulkanContext, size, imageData, setting);
    mAssets[texture->mID] = texture;
    return texture;
}
void MTextureManager::Update(std::shared_ptr<MTexture> texture)
{
    mAssets[texture->mID] = texture;
}
void MTextureManager::CreateVulkanResources(std::shared_ptr<MTexture> texture)
{
    if (texture->mImage)
    {
        VkImage image = texture->mImage;
        vmaDestroyImage(mVulkanContext->GetVmaAllocator(), image, texture->mAllocation);
    }
    vk::ImageCreateInfo imageCreateInfo{};
    texture->mSetting.mipmapLevels = std::min(
        static_cast<uint32_t>(std::floor(std::log2(std::max(texture->mSize.width, texture->mSize.height)))) + 1,
        texture->mSetting.mipmapLevels);
    imageCreateInfo.setImageType(TextureTypeToImageType(texture->mSetting.ImageType))
        .setExtent({texture->mSize.width, texture->mSize.height, 1})
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
}
void MTextureManager::Write(std::shared_ptr<MTexture> texture)
{
    mCommandBuffer->reset();
    mVulkanContext->GetDevice().resetFences(mFence.get());
    vk::Buffer stagingBuffer;
    vk::BufferCreateInfo stagingBufferCreateInfo{};
    stagingBufferCreateInfo
        .setSize(texture->mSize.width * texture->mSize.height * PickPixelSize(texture->mSetting.format).second)
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
    memcpy(stagingAllocationInfo.pMappedData, texture->mImageData.data(),
           texture->mSize.width * texture->mSize.height * PickPixelSize(texture->mSetting.format).second);
    mCommandBuffer->begin(vk::CommandBufferBeginInfo{});
    {
        // 转换图像布局UNDEFINED → TRANSFER_DST
        vk::ImageMemoryBarrier imageBarrier{};
        imageBarrier.setImage(texture->mImage)
            .setOldLayout(vk::ImageLayout::eUndefined)
            .setNewLayout(vk::ImageLayout::eTransferDstOptimal)
            .setSrcAccessMask(vk::AccessFlagBits::eNone)
            .setDstAccessMask(vk::AccessFlagBits::eTransferWrite)
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
            .setImageExtent({texture->mSize.width, texture->mSize.height, 1});
        mCommandBuffer->copyBufferToImage(stagingBuffer, texture->mImage, vk::ImageLayout::eTransferDstOptimal,
                                          {bufferImageCopy});
        // 如果需要生成mipmap，则生成mipmap
        for (uint32_t mipmapLevel = 1; mipmapLevel < texture->mSetting.mipmapLevels; mipmapLevel++)
        {
            imageBarrier.setOldLayout(vk::ImageLayout::eTransferDstOptimal)
                .setNewLayout(vk::ImageLayout::eTransferSrcOptimal)
                .setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
                .setDstAccessMask(vk::AccessFlagBits::eTransferRead)
                .setSubresourceRange(vk::ImageSubresourceRange()
                                         .setAspectMask(GuessImageAspectFlags(texture->mSetting.format))
                                         .setBaseMipLevel(mipmapLevel - 1)
                                         .setLevelCount(1)
                                         .setBaseArrayLayer(0)
                                         .setLayerCount(texture->mSetting.arrayLayers));
            mCommandBuffer->pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer,
                                            {}, {}, {}, {imageBarrier});
            int32_t mipWidth = std::max(1u, texture->mSize.width >> (mipmapLevel - 1));
            int32_t mipHeight = std::max(1u, texture->mSize.height >> (mipmapLevel - 1));
            vk::ImageBlit imageBlit{};
            imageBlit.setSrcOffsets({vk::Offset3D{0, 0, 0}, vk::Offset3D{mipWidth, mipHeight, 1}})
                .setSrcSubresource(vk::ImageSubresourceLayers()
                                       .setAspectMask(GuessImageAspectFlags(texture->mSetting.format))
                                       .setMipLevel(mipmapLevel - 1)
                                       .setBaseArrayLayer(0)
                                       .setLayerCount(texture->mSetting.arrayLayers))
                .setDstOffsets(
                    {vk::Offset3D{0, 0, 0}, vk::Offset3D{std::max(mipWidth / 2, 1), std::max(mipHeight / 2, 1), 1}})
                .setDstSubresource(vk::ImageSubresourceLayers()
                                       .setAspectMask(GuessImageAspectFlags(texture->mSetting.format))
                                       .setMipLevel(mipmapLevel)
                                       .setBaseArrayLayer(0)
                                       .setLayerCount(texture->mSetting.arrayLayers));
            mCommandBuffer->blitImage(texture->mImage, vk::ImageLayout::eTransferSrcOptimal, texture->mImage,
                                      vk::ImageLayout::eTransferDstOptimal, {imageBlit}, vk::Filter::eLinear);

            imageBarrier.setOldLayout(vk::ImageLayout::eTransferSrcOptimal)
                .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
                .setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
                .setDstAccessMask(vk::AccessFlagBits::eShaderRead)
                .setSubresourceRange(vk::ImageSubresourceRange()
                                         .setAspectMask(GuessImageAspectFlags(texture->mSetting.format))
                                         .setBaseMipLevel(mipmapLevel - 1)
                                         .setLevelCount(1)
                                         .setBaseArrayLayer(0)
                                         .setLayerCount(texture->mSetting.arrayLayers));
            mCommandBuffer->pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
                                            vk::PipelineStageFlagBits::eFragmentShader, {}, {}, {}, {imageBarrier});
        }
        // 转换图像布局：TRANSFER_DST → SHADER_READ
        imageBarrier.setOldLayout(vk::ImageLayout::eTransferDstOptimal)
            .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
            .setSrcQueueFamilyIndex(mVulkanContext->GetQueueFamilyIndicates().graphicsFamily.value())
            .setDstQueueFamilyIndex(mVulkanContext->GetQueueFamilyIndicates().graphicsFamily.value())
            .setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
            .setDstAccessMask(vk::AccessFlagBits::eShaderRead)
            .setSubresourceRange(vk::ImageSubresourceRange()
                                     .setAspectMask(GuessImageAspectFlags(texture->mSetting.format))
                                     .setBaseMipLevel(texture->mSetting.mipmapLevels - 1)
                                     .setLevelCount(1)
                                     .setBaseArrayLayer(0)
                                     .setLayerCount(texture->mSetting.arrayLayers));
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
    // 创建缩略图描述集
    if (!texture->mSetting.isDepthStencil)
    {
        texture->mThumbnailDescriptorSet =
            ImGui_ImplVulkan_AddTexture(texture->mSampler.get(), texture->mImageView.get(),
                                        static_cast<VkImageLayout>(vk::ImageLayout::eShaderReadOnlyOptimal));
    }
}
void MTextureManager::CreateDefault()
{
    auto whiteTexture = CreateWhiteTexture();
    auto blackTexture = CreateBlackTexture();
    auto magentaTexture = CreateMagentaTexture();
    auto normalTexture = CreateNormalTexture();
    auto emissiveTexture = CreateEmissiveTexture();
    auto albedoTexture = CreateAlbedoTexture();
    auto armTexture = CreateARMTexture();
    auto environmentMap = CreateEnvironmentMap();
    auto irradianceMap = CreateIrradianceMap();
    auto brdfLUT = CreateBRDFLUT();
    Remove(whiteTexture->mID);
    Remove(blackTexture->mID);
    Remove(magentaTexture->mID);
    Remove(normalTexture->mID);
    Remove(emissiveTexture->mID);
    Remove(albedoTexture->mID);
    Remove(armTexture->mID);
    Remove(environmentMap->mID);
    Remove(irradianceMap->mID);
    Remove(brdfLUT->mID);
    whiteTexture->mID = mDefaultTextures[DefaultTextureType::White];
    blackTexture->mID = mDefaultTextures[DefaultTextureType::Black];
    magentaTexture->mID = mDefaultTextures[DefaultTextureType::Magenta];
    normalTexture->mID = mDefaultTextures[DefaultTextureType::Normal];
    emissiveTexture->mID = mDefaultTextures[DefaultTextureType::Emissive];
    albedoTexture->mID = mDefaultTextures[DefaultTextureType::Albedo];
    armTexture->mID = mDefaultTextures[DefaultTextureType::ARM];
    environmentMap->mID = mDefaultTextures[DefaultTextureType::EnvironmentMap];
    irradianceMap->mID = mDefaultTextures[DefaultTextureType::IrradianceMap];
    brdfLUT->mID = mDefaultTextures[DefaultTextureType::BRDFLUT];

    mAssets[mDefaultTextures[DefaultTextureType::White]] = whiteTexture;
    mAssets[mDefaultTextures[DefaultTextureType::Black]] = blackTexture;
    mAssets[mDefaultTextures[DefaultTextureType::Magenta]] = magentaTexture;
    mAssets[mDefaultTextures[DefaultTextureType::Normal]] = normalTexture;
    mAssets[mDefaultTextures[DefaultTextureType::Emissive]] = emissiveTexture;
    mAssets[mDefaultTextures[DefaultTextureType::Albedo]] = albedoTexture;
    mAssets[mDefaultTextures[DefaultTextureType::ARM]] = armTexture;
    mAssets[mDefaultTextures[DefaultTextureType::EnvironmentMap]] = environmentMap;
    mAssets[mDefaultTextures[DefaultTextureType::IrradianceMap]] = irradianceMap;
    mAssets[mDefaultTextures[DefaultTextureType::BRDFLUT]] = brdfLUT;
}
std::shared_ptr<MTexture> MTextureManager::CreateWhiteTexture()
{
    auto whiteTextureSetting = MTextureSetting();
    auto whiteTexture = Create("Default White Texture", {1, 1, 4}, GetWhiteData(), whiteTextureSetting);
    CreateVulkanResources(whiteTexture);
    Write(whiteTexture);
    return whiteTexture;
}
std::shared_ptr<MTexture> MTextureManager::CreateMagentaTexture()
{
    auto magentaTextureSetting = MTextureSetting();
    auto magentaTexture = Create("Default Magenta Texture", {1, 1, 4}, GetMagentaData(), magentaTextureSetting);
    CreateVulkanResources(magentaTexture);
    Write(magentaTexture);
    return magentaTexture;
}
std::shared_ptr<MTexture> MTextureManager::CreateBlackTexture()
{
    auto blackTextureSetting = MTextureSetting();
    auto blackTexture = Create("Default Black Texture", {1, 1, 4}, GetBlackData(), blackTextureSetting);
    CreateVulkanResources(blackTexture);
    Write(blackTexture);
    return blackTexture;
}
std::shared_ptr<MTexture> MTextureManager::CreateNormalTexture()
{
    auto normalTextureSetting = MTextureSetting();
    auto normalTexture = Create("Default Normal Texture", {1, 1, 4}, GetNormalData(), normalTextureSetting);
    CreateVulkanResources(normalTexture);
    Write(normalTexture);
    return normalTexture;
}
std::shared_ptr<MTexture> MTextureManager::CreateEmissiveTexture()
{
    auto emissiveTextureSetting = MTextureSetting();
    auto emissiveTexture = Create("Default Emissive Texture", {1, 1, 4}, GetEmissiveData(), emissiveTextureSetting);
    CreateVulkanResources(emissiveTexture);
    Write(emissiveTexture);
    return emissiveTexture;
}
std::shared_ptr<MTexture> MTextureManager::CreateAlbedoTexture()
{
    auto albedoTextureSetting = MTextureSetting();
    auto albedoTexture = Create("Default Albedo Texture", {1, 1, 4}, GetAlbedoData(), albedoTextureSetting);
    CreateVulkanResources(albedoTexture);
    Write(albedoTexture);
    return albedoTexture;
}
std::shared_ptr<MTexture> MTextureManager::CreateARMTexture()
{
    auto armTextureSetting = MTextureSetting();
    auto armTexture = Create("Default ARM Texture", {1, 1, 4}, GetARMData(), armTextureSetting);
    CreateVulkanResources(armTexture);
    Write(armTexture);
    return armTexture;
}
std::shared_ptr<MTexture> MTextureManager::GetDefaultTexture(DefaultTextureType type) const
{
    if (mDefaultTextures.find(type) != mDefaultTextures.end())
    {
        return Get(mDefaultTextures.at(type));
    }
    LogError("Default texture type {} not found", static_cast<int>(type));
    return nullptr;
}
std::shared_ptr<MTexture> MTextureManager::CreateColorAttachment(uint32_t width, uint32_t height)
{
    auto colorAttachmentSetting = MTextureSetting();
    colorAttachmentSetting.isRenderTarget = true;
    colorAttachmentSetting.isShaderResource = true;
    colorAttachmentSetting.format = vk::Format::eR32G32B32A32Sfloat;
    colorAttachmentSetting.ImageType = vk::ImageViewType::e2D;
    auto colorAttachment = Create("Color Attachment", {width, height, 4}, {}, colorAttachmentSetting);
    CreateVulkanResources(colorAttachment);
    return colorAttachment;
}
std::shared_ptr<MTexture> MTextureManager::CreateDepthStencilAttachment(uint32_t width, uint32_t height)
{
    auto depthStencilAttachmentSetting = MTextureSetting();
    depthStencilAttachmentSetting.isRenderTarget = true;
    depthStencilAttachmentSetting.isShaderResource = false;
    depthStencilAttachmentSetting.isDepthStencil = true;
    depthStencilAttachmentSetting.format = vk::Format::eD32SfloatS8Uint;
    depthStencilAttachmentSetting.ImageType = vk::ImageViewType::e2D;
    auto depthStencilAttachment =
        Create("Depth Stencil Attachment", {width, height, 4}, {}, depthStencilAttachmentSetting);
    CreateVulkanResources(depthStencilAttachment);
    return depthStencilAttachment;
}
std::shared_ptr<MTexture> MTextureManager::CreateEnvironmentMap()
{
    auto environmentMapSetting = MTextureSetting();
    environmentMapSetting.isShaderResource = true;
    environmentMapSetting.format = vk::Format::eR32G32B32A32Sfloat;
    environmentMapSetting.ImageType = vk::ImageViewType::e2D;
    environmentMapSetting.mipmapLevels = 9;
    environmentMapSetting.maxLod = 8;
    auto &&[W, H, C, data] = Utils::ImageUtil::LoadHDRImage("Engine/Textures/EnvironmentMap.hdr");
    auto environmentMap =
        Create("Environment Map", {static_cast<uint32_t>(W), static_cast<uint32_t>(H), static_cast<uint32_t>(C)}, data,
               environmentMapSetting);
    CreateVulkanResources(environmentMap);
    Write(environmentMap);
    return environmentMap;
}
std::shared_ptr<MTexture> MTextureManager::CreateIrradianceMap()
{
    auto irradianceMapSetting = MTextureSetting();
    irradianceMapSetting.isShaderResource = true;
    irradianceMapSetting.format = vk::Format::eR32G32B32A32Sfloat;
    irradianceMapSetting.ImageType = vk::ImageViewType::e2D;
    irradianceMapSetting.mipmapLevels = 1;
    auto &&[W, H, C, data] = Utils::ImageUtil::LoadHDRImage("Engine/Textures/IrradianceMap.hdr");
    auto irradianceMap =
        Create("Irradiance Map", {static_cast<uint32_t>(W), static_cast<uint32_t>(H), static_cast<uint32_t>(C)}, data,
               irradianceMapSetting);
    CreateVulkanResources(irradianceMap);
    Write(irradianceMap);
    return irradianceMap;
}
std::shared_ptr<MTexture> MTextureManager::CreateBRDFLUT()
{
    ktxTexture *brdfLUTTexture = nullptr;
    if (ktxTexture_CreateFromNamedFile("E:/Code/MEngineV3/Resource/Engine/Textures/BRDFLUT.ktx",
                                       KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &brdfLUTTexture) != KTX_SUCCESS)
    {
        LogError("Failed to load BRDF LUT texture");
        throw std::runtime_error("Failed to load BRDF LUT texture");
    }
    auto brdfLUTSetting = MTextureSetting();
    brdfLUTSetting.isShaderResource = true;
    brdfLUTSetting.format = static_cast<vk::Format>(ktxTexture_GetVkFormat(ktxTexture(brdfLUTTexture)));
    brdfLUTSetting.mipmapLevels = brdfLUTTexture->numLevels;
    auto &&[C, S] = PickPixelSize(brdfLUTSetting.format);
    auto brdfLUT = Create("BRDF LUT",
                          {
                              static_cast<uint32_t>(brdfLUTTexture->baseWidth),
                              static_cast<uint32_t>(brdfLUTTexture->baseHeight),
                              static_cast<uint32_t>(C),
                          },
                          std::vector<uint8_t>(brdfLUTTexture->pData, brdfLUTTexture->pData + brdfLUTTexture->dataSize),
                          brdfLUTSetting);
    CreateVulkanResources(brdfLUT);
    Write(brdfLUT);
    return brdfLUT;
}
} // namespace MEngine::Core::Manager
