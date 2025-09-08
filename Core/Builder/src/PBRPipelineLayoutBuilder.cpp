#include "PBRPipelineLayoutBuilder.hpp"
#include "PipelineLayoutType.hpp"
#include "UUIDGenerator.hpp"
namespace MEngine::Core
{
void PBRPipelineLayoutBuilder::Reset()
{
}

std::unique_ptr<PipelineLayout> PBRPipelineLayoutBuilder::Build()
{
    mPipelineLayoutCreateInfo.setSetLayouts(mSetLayouts).setPushConstantRanges(mPushConstantRanges);
    vk::UniquePipelineLayout pipelineLayout =
        mVulkanContext->GetDevice().createPipelineLayoutUnique(mPipelineLayoutCreateInfo);
    if (!pipelineLayout)
    {
        throw std::runtime_error("Failed to create PBR Pipeline Layout");
    }
    return std::make_unique<PipelineLayout>(UUIDGenerator::Instance().Create(), "PBR Pipeline Layout",
                                            PipelineLayoutType::PBR, std::move(pipelineLayout));
}
void PBRPipelineLayoutBuilder::SetBindings()
{
    PipelineLayoutBuilder::SetBindings();
    // set: 1
    mPBRPipelineLayoutBindings = {
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
    };
}
void PBRPipelineLayoutBuilder::SetLayout()
{
    PipelineLayoutBuilder::SetLayout();
    // set:1
    vk::DescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.setBindings(mPBRPipelineLayoutBindings);
    mPBRPipelineLayoutDescriptorSetLayout = mVulkanContext->GetDevice().createDescriptorSetLayoutUnique(layoutInfo);
}

void PBRPipelineLayoutBuilder::SetPushConstants()
{
    PipelineLayoutBuilder::SetPushConstants();
}

} // namespace MEngine::Core