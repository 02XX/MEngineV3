#include "GraphicPipelineBuilder.hpp"
#include "GraphicPipeline.hpp"
#include "Vertex.hpp"
#include <memory>
namespace MEngine::Core
{
void GraphicPipelineBuilder::Reset()
{
    mGraphicPipeline = std::unique_ptr<GraphicPipeline>(new GraphicPipeline());
}
std::unique_ptr<GraphicPipeline> GraphicPipelineBuilder::Build()
{
    auto &&[renderPass, subPass] = mRenderPassManager->GetRenderPass(mGraphicPipeline->mRenderPassType);
    auto pipelineLayout = mPipelineLayoutManager->GetByType(mGraphicPipeline->mPipelineLayoutType);
    mGraphicPipeline->mCreateInfo.setStages(mGraphicPipeline->mShaderStages)
        .setPVertexInputState(&mGraphicPipeline->mVertexInputInfo)
        .setPInputAssemblyState(&mGraphicPipeline->mInputAssemblyState)
        .setPRasterizationState(&mGraphicPipeline->mRasterizationState)
        .setPViewportState(&mGraphicPipeline->mViewportState)
        .setPMultisampleState(&mGraphicPipeline->mMultisampleState)
        .setPDepthStencilState(&mGraphicPipeline->mDepthStencilState)
        .setPColorBlendState(&mGraphicPipeline->mColorBlendState)
        .setPDynamicState(&mGraphicPipeline->mDynamicState)
        .setLayout(pipelineLayout->GetPipelineLayout())
        .setRenderPass(renderPass)
        .setSubpass(subPass);
    auto pipeline =
        mVulkanContext->GetDevice().createGraphicsPipelineUnique(nullptr, mGraphicPipeline->mCreateInfo, nullptr);
    if (pipeline.result != vk::Result::eSuccess)
    {
        throw std::runtime_error("Failed to create Graphic Pipeline");
    }
    mGraphicPipeline->mPipeline = std::move(pipeline.value);
    return std::move(mGraphicPipeline);
}
void GraphicPipelineBuilder::SetVertexInputState()
{
    mGraphicPipeline->mVertexBindings = Vertex::GetVertexInputBindingDescription();
    mGraphicPipeline->mVertexAttributes = Vertex::GetVertexInputAttributeDescription();
    mGraphicPipeline->mVertexInputInfo.setVertexBindingDescriptions(mGraphicPipeline->mVertexBindings)
        .setVertexAttributeDescriptions(mGraphicPipeline->mVertexAttributes);
}
void GraphicPipelineBuilder::SetInputAssemblyState()
{
    mGraphicPipeline->mInputAssemblyState.setTopology(vk::PrimitiveTopology::eTriangleList)
        .setPrimitiveRestartEnable(VK_FALSE);
}

void GraphicPipelineBuilder::SetRasterizationState()
{
    mGraphicPipeline->mRasterizationState.setDepthClampEnable(vk::False)
        .setRasterizerDiscardEnable(vk::False)
        .setPolygonMode(vk::PolygonMode::eFill)
        .setLineWidth(1.0f)
        .setCullMode(vk::CullModeFlagBits::eBack)
        .setFrontFace(vk::FrontFace::eCounterClockwise)
        .setDepthBiasEnable(vk::False);
}
void GraphicPipelineBuilder::SetViewportState()
{
    mGraphicPipeline->mViewportState.setViewportCount(1).setScissorCount(1);
}
void GraphicPipelineBuilder::SetMultiSampleState()
{
    mGraphicPipeline->mMultisampleState.setSampleShadingEnable(vk::False)
        .setRasterizationSamples(vk::SampleCountFlagBits::e1)
        .setMinSampleShading(1.0f)
        .setPSampleMask(nullptr)
        .setAlphaToCoverageEnable(vk::False)
        .setAlphaToOneEnable(vk::False);
}
void GraphicPipelineBuilder::SetDepthStencilState()
{
    mGraphicPipeline->mDepthStencilState.setDepthTestEnable(vk::True)
        .setDepthWriteEnable(vk::True)
        .setDepthCompareOp(vk::CompareOp::eLess)
        .setDepthBoundsTestEnable(vk::False)
        .setMinDepthBounds(0.0f)
        .setMaxDepthBounds(1.0f)
        .setStencilTestEnable(vk::False);
}
void GraphicPipelineBuilder::SetColorBlendState()
{
    mGraphicPipeline->mColorBlendAttachments = {
        vk::PipelineColorBlendAttachmentState()
            .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                               vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
            .setBlendEnable(vk::False)};
    mGraphicPipeline->mColorBlendState.setLogicOpEnable(vk::False)
        .setLogicOp(vk::LogicOp::eCopy)
        .setAttachments(mGraphicPipeline->mColorBlendAttachments)
        .setBlendConstants({0.0f, 0.0f, 0.0f, 0.0f});
}
void GraphicPipelineBuilder::SetDynamicState()
{
    mGraphicPipeline->mDynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    mGraphicPipeline->mDynamicState.setDynamicStates(mGraphicPipeline->mDynamicStates);
}
} // namespace MEngine::Core