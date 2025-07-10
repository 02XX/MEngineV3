#pragma once
#include "MAsset.hpp"
#include "MManager_fwd.hpp"
#include "VulkanContext.hpp"
#include <cstdint>
#include <filesystem>
#include <imgui.h>
#include <imgui_impl_vulkan.h>
#include <memory>
#include <nlohmann/json_fwd.hpp>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace MEngine::Core::Asset
{
class MTextureSetting final : public MAssetSetting
{
  public:
    vk::ImageViewType ImageType = vk::ImageViewType::e2D;
    uint32_t mipmapLevels = 1;
    uint32_t arrayLayers = 1;
    vk::Format format = vk::Format::eR8G8B8A8Srgb;
    vk::SampleCountFlagBits sampleCount = vk::SampleCountFlagBits::e1;
    bool isShaderResource = true;
    bool isRenderTarget = false;
    bool isDepthStencil = false;
    bool isUAV = false;
    bool isShadingRateSurface = false;
    bool isTypeless = false;
    bool isTiled = false;
    vk::SamplerAddressMode addressModeU = vk::SamplerAddressMode::eRepeat;
    vk::SamplerAddressMode addressModeV = vk::SamplerAddressMode::eRepeat;
    vk::SamplerAddressMode addressModeW = vk::SamplerAddressMode::eRepeat;
    vk::Filter minFilter = vk::Filter::eLinear;
    vk::Filter magFilter = vk::Filter::eLinear;
    vk::SamplerMipmapMode mipmapMode = vk::SamplerMipmapMode::eLinear;
    float mipLodBias = 0.0f;
    float minLod = 0.0f;
    float maxLod = 0.0f;
    vk::BorderColor borderColor = vk::BorderColor::eFloatOpaqueBlack;
    bool compareEnable = false;
    vk::CompareOp compareOp = vk::CompareOp::eAlways;
    bool anisotropyEnable = false;
    float maxAnisotropy = 1.0f;
    vk::Bool32 unnormalizedCoordinates = vk::False;

  public:
    ~MTextureSetting() override = default;
};
struct TextureSize
{
    uint32_t width;
    uint32_t height;
    uint32_t channels;
};
class MTexture final : public MAsset
{
    friend class nlohmann::adl_serializer<MTexture>;
    friend class Manager::MTextureManager;

  private:
    std::shared_ptr<VulkanContext> mVulkanContext{};
    TextureSize mSize{};
    std::vector<uint8_t> mImageData{};
    MTextureSetting mSetting{};

    // GPU resources
    vk::Image mImage{};
    vk::UniqueImageView mImageView{};
    vk::UniqueSampler mSampler{};
    VmaAllocation mAllocation{};
    VmaAllocationInfo mAllocationInfo{};
    vk::DescriptorSet mThumbnailDescriptorSet{};

  public:
    MTexture(const UUID &id, const std::string &name, std::shared_ptr<VulkanContext> vulkanContext, TextureSize size,
             const std::vector<uint8_t> &imageData, const MTextureSetting &setting)
        : MAsset(id, name), mSetting(setting), mVulkanContext(vulkanContext), mImageData(imageData), mSize(size)
    {
        mType = MAssetType::Texture;
        mState = MAssetState::Unloaded;
    }
    ~MTexture() override
    {
        if (mImage)
        {
            VkImage image = mImage;
            vmaDestroyImage(mVulkanContext->GetVmaAllocator(), image, mAllocation);
        }
        // TODO: Remove ImGui texture Safely!!!!
        //  if (mThumbnailDescriptorSet)
        //  {
        //      ImGui_ImplVulkan_RemoveTexture(mThumbnailDescriptorSet);
        //  }
    }
    inline const TextureSize &GetSize() const
    {
        return mSize;
    }
    inline const vk::Image &GetImage() const
    {
        return mImage;
    }
    inline const vk::ImageView &GetImageView() const
    {
        return *mImageView;
    }
    inline const vk::Sampler &GetSampler() const
    {
        return *mSampler;
    }
    inline const MTextureSetting &GetSetting() const
    {
        return mSetting;
    }
    inline vk::DescriptorSet GetImGuiTextureID() const
    {
        return mThumbnailDescriptorSet;
    }
    inline void SetImGuiTextureID(vk::DescriptorSet thumbnailDescriptorSet)
    {
        mThumbnailDescriptorSet = thumbnailDescriptorSet;
    }
};

} // namespace MEngine::Core::Asset