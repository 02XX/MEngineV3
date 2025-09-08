#include "PipelineLayoutBuilder.hpp"
namespace MEngine::Core
{
void PipelineLayoutBuilder::Reset()
{
    mPipelineLayoutCreateInfo = vk::PipelineLayoutCreateInfo();
    mSetLayouts.clear();
    mPushConstantRanges.clear();
    mPipelineLayoutBindings.clear();
    mPipelineLayoutDescriptorSetLayout.reset();
}
void PipelineLayoutBuilder::SetBindings()
{
    // set:0
    mPipelineLayoutBindings = std::vector<vk::DescriptorSetLayoutBinding>{
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
    };
}
void PipelineLayoutBuilder::SetLayout()
{
    vk::DescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.setBindings(mPipelineLayoutBindings);
    mPipelineLayoutDescriptorSetLayout = mVulkanContext->GetDevice().createDescriptorSetLayoutUnique(layoutInfo);
}
void PipelineLayoutBuilder::SetPushConstants()
{
    vk::PushConstantRange mPushConstantRange{};
    mPushConstantRange.setSize(64).setOffset(0).setStageFlags(vk::ShaderStageFlagBits::eVertex |
                                                              vk::ShaderStageFlagBits::eFragment);
    mPushConstantRanges.push_back(mPushConstantRange);
}
} // namespace MEngine::Core