#pragma once
#include "MAsset.hpp"
#include "MManager_fwd.hpp"
#include "VMA.hpp"
#include "VulkanContext.hpp"

#include "Math.hpp"
#include <glm/detail/qualifier.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <memory>
#include <nlohmann/json_fwd.hpp>
#include <ranges>
#include <string>
#include <vector>
#include <vulkan/vulkan_handles.hpp>

namespace MEngine::Core::Asset
{
struct PipelineType
{
    static constexpr const char *ForwardOpaquePBR = "ForwardOpaquePBR";
    static constexpr const char *ForwardTransparentPBR = "ForwardTransparentPBR";
};
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
    RenderPassType RenderPassType = RenderPassType::ForwardComposition;

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
    // set:1
    std::vector<vk::DescriptorSetLayoutBinding> MaterialDescriptorSetLayoutBindings{

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
                                       vk::ShaderStageFlagBits::eFragment}};
    ~MPipelineSetting() override = default;
};
class MPipeline : public MAsset
{
    friend class nlohmann::adl_serializer<MPipeline>;
    friend class Manager::MPipelineManager;

  private:
    std::shared_ptr<VulkanContext> mVulkanContext;
    MPipelineSetting mSetting{};
    // vulkan
    vk::UniqueShaderModule mVertexShaderModule;
    vk::UniqueShaderModule mFragmentShaderModule;
    vk::UniquePipelineLayout mPipelineLayout;
    vk::UniquePipeline mPipeline;
    vk::UniqueDescriptorSetLayout mMaterialDescriptorSetLayouts;

  public:
    MPipeline(const UUID &id, const std::string &name, std::shared_ptr<VulkanContext> vulkanContext,
              const MPipelineSetting &setting)
        : MAsset(id, name), mVulkanContext(vulkanContext), mSetting(setting)
    {
        mType = MAssetType::Shader;
    }
    ~MPipeline() override = default;
    inline const MPipelineSetting &GetSetting() const
    {
        return mSetting;
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
    inline const vk::DescriptorSetLayout GetMaterialDescriptorSetLayout() const
    {
        return mMaterialDescriptorSetLayouts.get();
    }
    inline const std::vector<vk::DescriptorSetLayoutBinding> &GetMaterialDescriptorSetLayoutBindings() const
    {
        return mSetting.MaterialDescriptorSetLayoutBindings;
    }
};
} // namespace MEngine::Core::Asset