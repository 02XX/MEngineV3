#include "PipelineLayoutBuilder.hpp"
namespace MEngine::Core
{
void PipelineLayoutBuilder::Reset()
{
    mPipelineLayout = std::unique_ptr<PipelineLayout>(new PipelineLayout());
}
std::unique_ptr<PipelineLayout> PipelineLayoutBuilder::Build()
{
    mPipelineLayout->mPipelineLayoutCreateInfo.setSetLayouts(mPipelineLayout->mSetLayouts)
        .setPushConstantRanges(mPipelineLayout->mPushConstantRanges);
    vk::UniquePipelineLayout pipelineLayout =
        mVulkanContext->GetDevice().createPipelineLayoutUnique(mPipelineLayout->mPipelineLayoutCreateInfo);
    if (!pipelineLayout)
    {
        throw std::runtime_error("Failed to create PBR Pipeline Layout");
    }
    mPipelineLayout->mPipelineLayout = std::move(pipelineLayout);
    return std::move(mPipelineLayout);
}
void PipelineLayoutBuilder::SetBindings()
{
    // set:0
    mPipelineLayout->mPipelineLayoutBindings.push_back({
        // set:0
        // Binding: 0 VP (View Projection Matrix)
        vk::DescriptorSetLayoutBinding{0, vk::DescriptorType::eUniformBuffer, 1,
                                       vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment},
        // Binding: 1 Light
        vk::DescriptorSetLayoutBinding{1, vk::DescriptorType::eUniformBuffer, 1,
                                       vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment},
        // Binding: 2 Environment Map
        vk::DescriptorSetLayoutBinding{2, vk::DescriptorType::eCombinedImageSampler, 1,
                                       vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment},
        // Binding: 3 Irradiance Map
        vk::DescriptorSetLayoutBinding{3, vk::DescriptorType::eCombinedImageSampler, 1,
                                       vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment},
        // Binding: 4 BRDF LUT
        vk::DescriptorSetLayoutBinding{4, vk::DescriptorType::eCombinedImageSampler, 1,
                                       vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment},
    });
}
void PipelineLayoutBuilder::SetLayout()
{
    vk::DescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.setBindings(mPipelineLayout->mPipelineLayoutBindings.back());
    mPipelineLayout->mPipelineLayoutDescriptorSetLayouts.push_back(
        mVulkanContext->GetDevice().createDescriptorSetLayoutUnique(layoutInfo));
}
void PipelineLayoutBuilder::SetPushConstants()
{
    vk::PushConstantRange pushConstantRange{};
    pushConstantRange.setSize(64).setOffset(0).setStageFlags(vk::ShaderStageFlagBits::eVertex |
                                                             vk::ShaderStageFlagBits::eFragment);
    mPipelineLayout->mPushConstantRanges.push_back(pushConstantRange);
}
} // namespace MEngine::Core