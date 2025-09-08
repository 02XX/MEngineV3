#pragma once
#include "Builder.hpp"
#include "IGraphicPipelineBuilder.hpp"
#include "IPipelineLayoutManager.hpp"
#include "IShaderManager.hpp"
#include "PipelineLayoutType.hpp"
#include "RenderPassManager.hpp"
#include "VulkanContext.hpp"
#include <memory>
#include <vector>

namespace MEngine::Core
{
class GraphicPipelineBuilder : public Builder<GraphicPipeline>, public virtual IGraphicPipelineBuilder
{
  protected:
    std::shared_ptr<VulkanContext> mVulkanContext;
    std::shared_ptr<IShaderManager> mShaderManager;
    std::shared_ptr<RenderPassManager> mRenderPassManager;
    std::shared_ptr<IPipelineLayoutManager> mPipelineLayoutManager;

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

    PipelineLayoutType mPipelineLayoutType{PipelineLayoutType::None};
    vk::RenderPass mRenderPass{nullptr};
    uint32_t mSubPass{0};

    std::string mName{"Unknown Pipeline"};

  public:
    GraphicPipelineBuilder(std::shared_ptr<VulkanContext> vulkanContext,
                           std::shared_ptr<IPipelineLayoutManager> pipelineLayoutManager,
                           std::shared_ptr<IShaderManager> shaderManager,
                           std::shared_ptr<RenderPassManager> renderPassManager)
        : mVulkanContext(vulkanContext), mShaderManager(shaderManager), mRenderPassManager(renderPassManager),
          mPipelineLayoutManager(pipelineLayoutManager)
    {
    }
    ~GraphicPipelineBuilder() override = default;
    virtual void Reset() override;
    virtual void SetVertexInputState() override;
    virtual void SetInputAssemblyState() override;
    virtual void SetRasterizationState() override;
    virtual void SetViewportState() override;
    virtual void SetMultiSampleState() override;
    virtual void SetDepthStencilState() override;
    virtual void SetColorBlendState() override;
    virtual void SetDynamicState() override;
};
} // namespace MEngine::Core