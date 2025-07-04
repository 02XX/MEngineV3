#pragma once
#include "MAsset.hpp"
#include "MAssetSetting.hpp"
#include "MManager_fwd.hpp"
#include "MTextureSetting.hpp"
#include "VulkanContext.hpp"
#include <filesystem>
#include <memory>
#include <nlohmann/json_fwd.hpp>
#include <vulkan/vulkan.hpp>

namespace MEngine::Core::Asset
{

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

  private:
    MTexture(const UUID &id, std::shared_ptr<VulkanContext> vulkanContext, const MTextureSetting &setting)
        : MAsset(id), mSetting(setting), mVulkanContext(vulkanContext)
    {
        mType = MAssetType::Texture;
        mState = MAssetState::Unloaded;
        mImagePath = "default_texture.png";
    }

  public:
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