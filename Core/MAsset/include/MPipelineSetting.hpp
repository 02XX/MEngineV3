#pragma once
#include "MAssetSetting.hpp"
#include "RenderPassManager.hpp"
#include <vector>
#include <vulkan/vulkan.hpp>

namespace MEngine::Core::Asset
{
class MPipelineSetting final : public MAssetSetting
{
  public:
    std::filesystem::path VertexShaderPath;
    std::filesystem::path FragmentShaderPath;
    // RenderPass
    Manager::RenderPassType mRenderPassType = Manager::RenderPassType::ForwardComposition;

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
} // namespace MEngine::Core::Asset