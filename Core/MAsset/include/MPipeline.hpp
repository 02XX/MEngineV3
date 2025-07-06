#pragma once
#include "MAsset.hpp"
#include "MManager_fwd.hpp"

#include <nlohmann/json_fwd.hpp>
#include <ranges>
#include <vector>

namespace MEngine::Core::Asset
{
enum class RenderPassType
{
    ShadowDepth,         // 生成所有光源的阴影贴图subpass0: 生成阴影贴图
    DeferredComposition, // Deferred, 延迟渲染Subpass0: GBuffer, Subpass1: Lighting
    ForwardComposition,  // Forward, 前向渲染subpass0: 不透明物体渲染 subpass1: 透明物体渲染，
                         // 创建多个MRT，Phong只渲染第一个MRT，PBR渲染所有MRT
    Sky,                 // 天空盒渲染subpass0: 天空盒渲染
    Transparent,         // Forward 透明物体渲染subpass0: 透明物体渲染
    PostProcess,         // 后处理渲染subpass0: 后处理渲染
    UI,                  // UI渲染subpass0: UI渲染
};
class MPipelineSetting final : public MAssetSetting
{
  public:
    std::filesystem::path VertexShaderPath;
    std::filesystem::path FragmentShaderPath;
    // RenderPass
    RenderPassType mRenderPassType = RenderPassType::ForwardComposition;

    //========== 5. 光栅化状态 ==========
    bool DepthClampEnable = false;
    bool RasterizerDiscardEnable = false;
    vk::PolygonMode PolygonMode = vk::PolygonMode::eFill;
    float LineWidth = 1.0f;
    vk::CullModeFlagBits CullMode = vk::CullModeFlagBits::eBack;
    vk::FrontFace FrontFace = vk::FrontFace::eClockwise;
    bool DepthBiasEnable = false;
    // ========= 6. 多重采样 ==========
    bool MultisamplingEnable = false;
    vk::SampleCountFlagBits SampleCount = vk::SampleCountFlagBits::e1;
    float MinSampleShading = 1.0f;
    bool AlphaToCoverageEnable = false;
    bool AlphaToOneEnable = false;
    // ========== 7. 深度模板测试 ==========
    bool DepthTestEnable = true;
    bool DepthWriteEnable = true;
    vk::CompareOp DepthCompareOp = vk::CompareOp::eLessOrEqual;
    bool DepthBoundsTestEnable = false;
    float MinDepthBounds = 0.0f;
    float MaxDepthBounds = 1.0f;
    bool StencilTestEnable = false;

    // ========== 8. 颜色混合状态 ==========
    bool ColorBlendingEnable = false;
    bool LogicOpEnable = false;
    vk::LogicOp LogicOp = vk::LogicOp::eCopy;

    std::vector<std::vector<vk::DescriptorSetLayoutBinding>> DescriptorSetLayoutBindings{
        // set:0
        {{
            // binding: 0 Camera
            vk::DescriptorSetLayoutBinding{0, vk::DescriptorType::eUniformBuffer, 1,
                                           vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment},
            // binding: 1 Light
            vk::DescriptorSetLayoutBinding{1, vk::DescriptorType::eUniformBuffer, 6,
                                           vk::ShaderStageFlagBits::eFragment},
        }},
        // set:1
        {{// Binding: 0 Parameters
          vk::DescriptorSetLayoutBinding{0, vk::DescriptorType::eUniformBuffer, 1,
                                         vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment},
          // Binding: 1 Albedo
          vk::DescriptorSetLayoutBinding{1, vk::DescriptorType::eCombinedImageSampler, 1,
                                         vk::ShaderStageFlagBits::eFragment},
          // Binding: 2 Normal Map
          vk::DescriptorSetLayoutBinding{2, vk::DescriptorType::eCombinedImageSampler, 1,
                                         vk::ShaderStageFlagBits::eFragment},
          // Binding: 3 ARM (Ambient Occlusion, Roughness, Metallic)
          vk::DescriptorSetLayoutBinding{3, vk::DescriptorType::eCombinedImageSampler, 1,
                                         vk::ShaderStageFlagBits::eFragment},
          // Binding: 4 Emissive
          vk::DescriptorSetLayoutBinding{4, vk::DescriptorType::eCombinedImageSampler, 1,
                                         vk::ShaderStageFlagBits::eFragment}}}};
    ~MPipelineSetting() override = default;
};
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