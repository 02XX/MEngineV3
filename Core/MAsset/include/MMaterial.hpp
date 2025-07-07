#pragma once

#include "MAsset.hpp"
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
    UUID mPipelineID{};
    MMaterialSetting mSetting{};
    vk::UniqueDescriptorSet mDescriptorSet;
  public:
    MMaterial(const UUID &id, const MMaterialSetting &setting) : MAsset(id), mSetting(setting)
    {
        mType = MAssetType::Material;
        mState = MAssetState::Unloaded;
    }
    inline UUID GetPipelineID() const
    {
        return mPipelineID;
    }
    inline void SetPipelineID(const UUID &pipelineID)
    {
        mPipelineID = pipelineID;
    }
    inline const vk::DescriptorSet GetDescriptorSet() const
    {
        return mDescriptorSet.get();
    }
};
} // namespace MEngine::Core::Asset
