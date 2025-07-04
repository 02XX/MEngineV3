#pragma once
#include "MAsset.hpp"
#include "MManager_fwd.hpp"
#include "MPipelineSetting.hpp"
#include <nlohmann/json_fwd.hpp>
#include <ranges>
#include <vector>

namespace MEngine::Core::Asset
{
class MPipeline : public MAsset
{
    friend class nlohmann::adl_serializer<MPipeline>;
    friend class Manager::MPipelineManager;

  private:
    MPipelineSetting mSetting;
    
    // vulkan
    vk::UniqueShaderModule mVertexShaderModule;
    vk::UniqueShaderModule mFragmentShaderModule;
    std::vector<vk::UniqueDescriptorSetLayout> mDescriptorSetLayouts;
    vk::UniquePipelineLayout mPipelineLayout;
    vk::UniquePipeline mPipeline;

  private:
    MPipeline(const UUID &id, const MPipelineSetting &setting) : MAsset(id), mSetting(setting)
    {
        mType = MAssetType::Shader;
    }

  public:
    ~MPipeline() override = default;
    inline const MPipelineSetting &GetSetting() const
    {
        return mSetting;
    }
    inline void SetSetting(const MPipelineSetting &setting)
    {
        mSetting = setting;
    }
    inline const vk::ShaderModule GetVertexShaderModule() const
    {
        return mVertexShaderModule.get();
    }
    inline const vk::ShaderModule GetFragmentShaderModule() const
    {
        return mFragmentShaderModule.get();
    }
    inline const vk::Pipeline GetPipeline() const
    {
        return mPipeline.get();
    }
    inline const vk::PipelineLayout GetPipelineLayout() const
    {
        return mPipelineLayout.get();
    }
    inline const std::vector<vk::DescriptorSetLayout> GetDescriptorSetLayouts() const
    {
        return mDescriptorSetLayouts | std::ranges::views::transform([](const auto &layout) { return layout.get(); }) |
               std::ranges::to<std::vector<vk::DescriptorSetLayout>>();
    }
};
} // namespace MEngine::Core::Asset