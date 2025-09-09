#include "PBRPipelineLayoutBuilder.hpp"
#include "UUIDGenerator.hpp"
namespace MEngine::Core
{
void PBRPipelineLayoutBuilder::SetBindings()
{
    PipelineLayoutBuilder::SetBindings();
    // set: 1
    mPipelineLayout->mPipelineLayoutBindings.push_back({
        // set:1
        // Binding: 0 PBR Parameters
        vk::DescriptorSetLayoutBinding{0, vk::DescriptorType::eUniformBuffer, 1,
                                       vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment},
        // Binding: 1 Albedo Map
        vk::DescriptorSetLayoutBinding{1, vk::DescriptorType::eCombinedImageSampler, 1,
                                       vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment},
        // Binding: 2 Normal Map
        vk::DescriptorSetLayoutBinding{2, vk::DescriptorType::eCombinedImageSampler, 1,
                                       vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment},
        // Binding: 3 Metallic Map
        vk::DescriptorSetLayoutBinding{3, vk::DescriptorType::eCombinedImageSampler, 1,
                                       vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment},
        // Binding: 4 Roughness Map
        vk::DescriptorSetLayoutBinding{4, vk::DescriptorType::eCombinedImageSampler, 1,
                                       vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment},
        // Binding: 5 AO Map
        vk::DescriptorSetLayoutBinding{5, vk::DescriptorType::eCombinedImageSampler, 1,
                                       vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment},
    });
}
void PBRPipelineLayoutBuilder::SetLayout()
{
    PipelineLayoutBuilder::SetLayout();
    // set:1
    vk::DescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.setBindings(mPipelineLayout->mPipelineLayoutBindings.back());
    mPipelineLayout->mPipelineLayoutDescriptorSetLayouts.push_back(
        mVulkanContext->GetDevice().createDescriptorSetLayoutUnique(layoutInfo));
}

void PBRPipelineLayoutBuilder::SetPushConstants()
{
    PipelineLayoutBuilder::SetPushConstants();
}
void PBRPipelineLayoutBuilder::SetPipelineLayoutType()
{
    mPipelineLayout->mPipelineLayoutType = PipelineLayoutType::PBR;
}
} // namespace MEngine::Core