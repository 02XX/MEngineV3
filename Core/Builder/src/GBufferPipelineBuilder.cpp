#include "GBufferPipelineBuilder.hpp"
#include "UUIDGenerator.hpp"

namespace MEngine::Core
{

void GBufferPipelineBuilder::SetName()
{
    mGraphicPipeline->mName = "GBuffer";
}
void GBufferPipelineBuilder::SetShader()
{
    // vertex
    auto vertexShader = mShaderManager->GetByName("gbuffer_vert");
    if (!vertexShader)
        throw std::runtime_error("Vertex Shader for GBuffer Pipeline not found");
    mGraphicPipeline->mShaderStages.push_back(vk::PipelineShaderStageCreateInfo{}
                                                  .setStage(vertexShader->GetStage())
                                                  .setModule(vertexShader->GetShaderModule())
                                                  .setPName("main"));
    // fragment
    auto fragmentShader = mShaderManager->GetByName("gbuffer_frag");
    if (!fragmentShader)
        throw std::runtime_error("Fragment Shader for GBuffer Pipeline not found");
    mGraphicPipeline->mShaderStages.push_back(vk::PipelineShaderStageCreateInfo{}
                                                  .setStage(fragmentShader->GetStage())
                                                  .setModule(fragmentShader->GetShaderModule())
                                                  .setPName("main"));
}
void GBufferPipelineBuilder::SetColorBlendState()
{
    // albedo
    mGraphicPipeline->mColorBlendAttachments.push_back(
        vk::PipelineColorBlendAttachmentState()
            .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                               vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
            .setBlendEnable(vk::False));
    // normal
    mGraphicPipeline->mColorBlendAttachments.push_back(
        vk::PipelineColorBlendAttachmentState()
            .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                               vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
            .setBlendEnable(vk::False));
    // arm
    mGraphicPipeline->mColorBlendAttachments.push_back(
        vk::PipelineColorBlendAttachmentState()
            .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                               vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
            .setBlendEnable(vk::False));
    // position
    mGraphicPipeline->mColorBlendAttachments.push_back(
        vk::PipelineColorBlendAttachmentState()
            .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                               vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
            .setBlendEnable(vk::False));
    // emissive
    mGraphicPipeline->mColorBlendAttachments.push_back(
        vk::PipelineColorBlendAttachmentState()
            .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                               vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
            .setBlendEnable(vk::False));
    mGraphicPipeline->mColorBlendState.setLogicOpEnable(vk::False)
        .setLogicOp(vk::LogicOp::eCopy)
        .setAttachments(mGraphicPipeline->mColorBlendAttachments)
        .setBlendConstants({0.0f, 0.0f, 0.0f, 0.0f});
}
void GBufferPipelineBuilder::SetLayout()
{
    mGraphicPipeline->mPipelineLayoutType = PipelineLayoutType::PBR;
    auto pipelineLayout = mPipelineLayoutManager->GetByType(mGraphicPipeline->mPipelineLayoutType);
    if (!pipelineLayout)
        throw std::runtime_error("Pipeline Layout for GBuffer Pipeline not found");
}
void GBufferPipelineBuilder::SetRenderPass()
{
    mGraphicPipeline->mRenderPassType = RenderPassType::GBuffer;
}

} // namespace MEngine::Core