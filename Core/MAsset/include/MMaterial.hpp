#pragma once

#include "MAsset.hpp"
#include "MPipeline.hpp"
#include <memory>
#include <vulkan/vulkan_handles.hpp>

namespace MEngine::Core::Asset
{

class MMaterialSetting : public MAssetSetting
{
  public:
};
class MMaterial : public MAsset
{
  protected:
    std::shared_ptr<MPipeline> mPipeline;
    MMaterialSetting mSetting{};
    vk::UniqueDescriptorSet mMaterialDescriptorSet;

  public:
    MMaterial(const UUID &id, const std::string &name, const MMaterialSetting &setting)
        : MAsset(id, name), mSetting(setting)
    {
        mType = MAssetType::Material;
        mState = MAssetState::Unloaded;
    }
    inline const std::shared_ptr<MPipeline> GetPipeline() const
    {
        return mPipeline;
    }
    inline void SetPipeline(const std::shared_ptr<MPipeline> &pipeline)
    {
        mPipeline = pipeline;
    }
    inline const vk::DescriptorSet GetMaterialDescriptorSet() const
    {
        return mMaterialDescriptorSet.get();
    }
    virtual std::span<const vk::DescriptorSetLayoutBinding> GetDescriptorSetLayoutBindings() const = 0;
};
} // namespace MEngine::Core::Asset
