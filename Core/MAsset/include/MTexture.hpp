#pragma once
#include "MAsset.hpp"
#include "MManager_fwd.hpp"
#include "VulkanContext.hpp"
#include <filesystem>
#include <memory>
#include <nlohmann/json_fwd.hpp>
#include <vulkan/vulkan.hpp>

namespace MEngine::Core::Asset
{
class MTextureSetting final : public MAssetSetting
{
  public:
    uint32_t width = 1;
    uint32_t height = 1;
    vk::ImageViewType ImageType = vk::ImageViewType::e2D;
    uint32_t mipmapLevels = 1;
    uint32_t arrayLayers = 1;
    vk::Format format = vk::Format::eR8G8B8A8Srgb;
    vk::SampleCountFlagBits sampleCount = vk::SampleCountFlagBits::e1;
    bool isShaderResource = true;
    bool isRenderTarget = false;
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
class MTexture final : public MAsset
{
    friend class nlohmann::adl_serializer<MTexture>;
    friend class Manager::MTextureManager;

  private:
    std::shared_ptr<VulkanContext> mVulkanContext{};
    std::filesystem::path mImagePath{};
    MTextureSetting mSetting{};

    // GPU resources
    vk::Image mImage{};
    vk::UniqueImageView mImageView{};
    vk::UniqueSampler mSampler{};
    VmaAllocation mAllocation{};
    VmaAllocationInfo mAllocationInfo{};

  public:
    MTexture(const UUID &id, std::shared_ptr<VulkanContext> vulkanContext, const MTextureSetting &setting)
        : MAsset(id), mSetting(setting), mVulkanContext(vulkanContext)
    {
        mType = MAssetType::Texture;
        mState = MAssetState::Unloaded;
        mImagePath = "default_texture.png";
    }
    ~MTexture() override
    {
        if (mImage)
        {
            VkImage image = mImage;
            vmaDestroyImage(mVulkanContext->GetVmaAllocator(), image, mAllocation);
        }
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
    inline void SetSetting(const MTextureSetting &setting)
    {
        mSetting = setting;
    }
    inline const std::filesystem::path &GetImagePath() const
    {
        return mImagePath;
    }
    inline void SetImagePath(const std::filesystem::path &path)
    {
        mImagePath = path;
    }
};

} // namespace MEngine::Core::Asset