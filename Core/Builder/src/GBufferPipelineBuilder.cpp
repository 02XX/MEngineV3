#include "GBufferPipelineBuilder.hpp"
#include "UUIDGenerator.hpp"

namespace MEngine::Core
{

void GBufferPipelineBuilder::Reset()
{
    GraphicPipelineBuilder::Reset();
}
void GBufferPipelineBuilder::SetName(const std::string &name)
{
    mName = name;
}
std::unique_ptr<GraphicPipeline> GBufferPipelineBuilder::Build()
{
    mCreateInfo.setStages(mShaderStages)
        .setPVertexInputState(&mVertexInputInfo)
        .setPInputAssemblyState(&mInputAssemblyState)
        .setPRasterizationState(&mRasterizationState)
        .setPViewportState(&mViewportState)
        .setPMultisampleState(&mMultisampleState)
        .setPDepthStencilState(&mDepthStencilState)
        .setPColorBlendState(&mColorBlendState)
        .setPDynamicState(&mDynamicState)
        .setLayout(mPipelineLayoutManager->GetByType(mPipelineLayoutType)->GetPipelineLayout())
        .setRenderPass(mRenderPass)
        .setSubpass(mSubPass);
    auto pipeline = mVulkanContext->GetDevice().createGraphicsPipelineUnique(nullptr, mCreateInfo, nullptr);
    if (pipeline.result != vk::Result::eSuccess)
    {
        throw std::runtime_error("Failed to create GBuffer Pipeline");
    }
    return std::make_unique<GraphicPipeline>(UUIDGenerator::Instance().Create(), mName, std::move(pipeline.value),
                                             mPipelineLayoutType, mSubPass);
}
void GBufferPipelineBuilder::SetShader()
{
    // vertex
    auto vertexShader = mShaderManager->GetByName("gbuffer_vert");
    if (!vertexShader)
        throw std::runtime_error("Vertex Shader for GBuffer Pipeline not found");
    mShaderStages.push_back(vk::PipelineShaderStageCreateInfo{}
                                .setStage(vertexShader->GetStage())
                                .setModule(vertexShader->GetShaderModule())
                                .setPName("main"));
    // fragment
    auto fragmentShader = mShaderManager->GetByName("gbuffer_frag");
    if (!fragmentShader)
        throw std::runtime_error("Fragment Shader for GBuffer Pipeline not found");
    mShaderStages.push_back(vk::PipelineShaderStageCreateInfo{}
                                .setStage(fragmentShader->GetStage())
                                .setModule(fragmentShader->GetShaderModule())
                                .setPName("main"));
}
void GBufferPipelineBuilder::SetColorBlendState()
{
    // albedo
    mColorBlendAttachments.push_back(
        vk::PipelineColorBlendAttachmentState()
            .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                               vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
            .setBlendEnable(vk::False));
    // normal
    mColorBlendAttachments.push_back(
        vk::PipelineColorBlendAttachmentState()
            .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                               vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
            .setBlendEnable(vk::False));
    // arm
    mColorBlendAttachments.push_back(
        vk::PipelineColorBlendAttachmentState()
            .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                               vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
            .setBlendEnable(vk::False));
    // position
    mColorBlendAttachments.push_back(
        vk::PipelineColorBlendAttachmentState()
            .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                               vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
            .setBlendEnable(vk::False));
    // emissive
    mColorBlendAttachments.push_back(
        vk::PipelineColorBlendAttachmentState()
            .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                               vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
            .setBlendEnable(vk::False));
    mColorBlendState.setLogicOpEnable(vk::False)
        .setLogicOp(vk::LogicOp::eCopy)
        .setAttachments(mColorBlendAttachments)
        .setBlendConstants({0.0f, 0.0f, 0.0f, 0.0f});
}
void GBufferPipelineBuilder::SetLayout()
{
    mPipelineLayoutType = PipelineLayoutType::PBR;
    auto pipelineLayout = mPipelineLayoutManager->GetByType(mPipelineLayoutType);
    if (!pipelineLayout)
        throw std::runtime_error("Pipeline Layout for GBuffer Pipeline not found");
}
void GBufferPipelineBuilder::SetRenderPass()
{
    auto [renderPass, subPass] = mRenderPassManager->GetRenderPass(RenderPassType::GBuffer);
    mRenderPass = renderPass;
    mSubPass = subPass;
}

} // namespace MEngine::Core