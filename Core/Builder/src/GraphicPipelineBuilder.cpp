#include "GraphicPipelineBuilder.hpp"
#include "Vertex.hpp"
namespace MEngine::Core
{
void GraphicPipelineBuilder::Reset()
{
    mCreateInfo = vk::GraphicsPipelineCreateInfo();
    mVertexBindings = vk::VertexInputBindingDescription();
    mVertexAttributes.clear();
    mVertexInputInfo = vk::PipelineVertexInputStateCreateInfo();
    mInputAssemblyState = vk::PipelineInputAssemblyStateCreateInfo();
    mRasterizationState = vk::PipelineRasterizationStateCreateInfo();
    mMultisampleState = vk::PipelineMultisampleStateCreateInfo();
    mDepthStencilState = vk::PipelineDepthStencilStateCreateInfo();
    mColorBlendState = vk::PipelineColorBlendStateCreateInfo();
    mColorBlendAttachments.clear();
    mDynamicState = vk::PipelineDynamicStateCreateInfo();
    mDynamicStates.clear();
    mShaderStages.clear();
    mRenderPass = nullptr;
    mSubPass = 0;
    mPipelineLayoutType = PipelineLayoutType::None;
}
void GraphicPipelineBuilder::SetVertexInputState()
{
    mVertexBindings = Vertex::GetVertexInputBindingDescription();
    mVertexAttributes = Vertex::GetVertexInputAttributeDescription();
    mVertexInputInfo.setVertexBindingDescriptions(mVertexBindings).setVertexAttributeDescriptions(mVertexAttributes);
}
void GraphicPipelineBuilder::SetInputAssemblyState()
{
    mInputAssemblyState.setTopology(vk::PrimitiveTopology::eTriangleList).setPrimitiveRestartEnable(VK_FALSE);
}

void GraphicPipelineBuilder::SetRasterizationState()
{
    mRasterizationState.setDepthClampEnable(vk::False)
        .setRasterizerDiscardEnable(vk::False)
        .setPolygonMode(vk::PolygonMode::eFill)
        .setLineWidth(1.0f)
        .setCullMode(vk::CullModeFlagBits::eBack)
        .setFrontFace(vk::FrontFace::eCounterClockwise)
        .setDepthBiasEnable(vk::False);
}
void GraphicPipelineBuilder::SetViewportState()
{
    mViewportState.setViewportCount(1).setScissorCount(1);
}
void GraphicPipelineBuilder::SetMultiSampleState()
{
    mMultisampleState.setSampleShadingEnable(vk::False)
        .setRasterizationSamples(vk::SampleCountFlagBits::e1)
        .setMinSampleShading(1.0f)
        .setPSampleMask(nullptr)
        .setAlphaToCoverageEnable(vk::False)
        .setAlphaToOneEnable(vk::False);
}
void GraphicPipelineBuilder::SetDepthStencilState()
{
    mDepthStencilState.setDepthTestEnable(vk::True)
        .setDepthWriteEnable(vk::True)
        .setDepthCompareOp(vk::CompareOp::eLess)
        .setDepthBoundsTestEnable(vk::False)
        .setMinDepthBounds(0.0f)
        .setMaxDepthBounds(1.0f)
        .setStencilTestEnable(vk::False);
}
void GraphicPipelineBuilder::SetColorBlendState()
{
    mColorBlendAttachments = {vk::PipelineColorBlendAttachmentState()
                                  .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                                                     vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
                                  .setBlendEnable(vk::False)};
    mColorBlendState.setLogicOpEnable(vk::False)
        .setLogicOp(vk::LogicOp::eCopy)
        .setAttachments(mColorBlendAttachments)
        .setBlendConstants({0.0f, 0.0f, 0.0f, 0.0f});
}
void GraphicPipelineBuilder::SetDynamicState()
{
    mDynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    mDynamicState.setDynamicStates(mDynamicStates);
}
} // namespace MEngine::Core