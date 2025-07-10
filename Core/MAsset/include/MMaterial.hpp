#pragma once

#include "MAsset.hpp"
#include "MPipeline.hpp"
#include "MPipelineManager.hpp"
#include <memory>
#include <vulkan/vulkan_handles.hpp>

namespace MEngine::Core::Asset
{
enum class MMaterialType
{
    Unknown, // Unknown Material Type
    PBR,     // Physically Based Rendering
    Unlit,   // Unlit Material
    Custom   // Custom Material
};
class MMaterialSetting : public MAssetSetting
{
  public:
};
class MMaterial : public MAsset
{
    friend class nlohmann::adl_serializer<MMaterial>;

  protected:
    std::string mPipelineName = PipelineType::ForwardOpaquePBR;
    MMaterialSetting mSetting{};
    vk::UniqueDescriptorSet mMaterialDescriptorSet;
    MMaterialType mMaterialType = MMaterialType::Unknown;
    // 导航属性
    std::shared_ptr<MPipeline> mPipeline;

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
    inline MMaterialType GetMaterialType() const
    {
        return mMaterialType;
    }
    inline void SetMaterialType(MMaterialType type)
    {
        mMaterialType = type;
    }
    virtual std::span<const vk::DescriptorSetLayoutBinding> GetDescriptorSetLayoutBindings() const = 0;
};
} // namespace MEngine::Core::Asset
