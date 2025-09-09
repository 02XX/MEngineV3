#pragma once
#include "Pipeline.hpp"
#include "RenderPassType.hpp"
namespace MEngine::Core
{
class GraphicPipeline final : public Pipeline
{
    friend class GraphicPipelineBuilder;
    friend class GBufferPipelineBuilder;
    friend class CustomGraphicPipelineBuilder;

  protected:
    vk::GraphicsPipelineCreateInfo mCreateInfo{};
    vk::VertexInputBindingDescription mVertexBindings{};
    std::vector<vk::VertexInputAttributeDescription> mVertexAttributes{};
    vk::PipelineVertexInputStateCreateInfo mVertexInputInfo{};
    vk::PipelineInputAssemblyStateCreateInfo mInputAssemblyState{};
    vk::PipelineRasterizationStateCreateInfo mRasterizationState{};
    vk::PipelineViewportStateCreateInfo mViewportState{};
    vk::PipelineMultisampleStateCreateInfo mMultisampleState{};
    vk::PipelineDepthStencilStateCreateInfo mDepthStencilState{};
    vk::PipelineColorBlendStateCreateInfo mColorBlendState{};
    std::vector<vk::PipelineColorBlendAttachmentState> mColorBlendAttachments{};
    vk::PipelineDynamicStateCreateInfo mDynamicState{};
    std::vector<vk::DynamicState> mDynamicStates{};

    std::vector<vk::PipelineShaderStageCreateInfo> mShaderStages{};
    RenderPassType mRenderPassType{RenderPassType::ForwardOpaque};

  protected:
    GraphicPipeline() : Pipeline()
    {
        mType = AssetType::Shader;
    }

  public:
    ~GraphicPipeline() override = default;
};
} // namespace MEngine::Core