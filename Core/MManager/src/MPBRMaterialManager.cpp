#include "MPBRMaterialManager.hpp"
#include "MPBRMaterial.hpp"
#include "MPipelineManager.hpp"
#include "UUID.hpp"
#include "VMA.hpp"
#include <cstring>
#include <memory>
#include <vector>
namespace MEngine::Core::Manager
{
std::shared_ptr<MPBRMaterial> MPBRMaterialManager::Create(const MPBRMaterialSetting &setting, const std::string &name)
{
    auto defaultTexture = mTextureManager->Get(UUID{});
    auto defaultPipeline = mPipelineManager->Get(PipelineType::ForwardOpaquePBR);
    auto pbrMaterial = std::make_shared<MPBRMaterial>(mUUIDGenerator->Create(), name, mVulkanContext, setting);
    pbrMaterial->mPipeline = defaultPipeline;
    pbrMaterial->mTextures.Albedo = defaultTexture;
    pbrMaterial->mTextures.Normal = defaultTexture;
    pbrMaterial->mTextures.ARM = defaultTexture;
    pbrMaterial->mTextures.Emissive = defaultTexture;
    pbrMaterial->mPipeline = defaultPipeline;
    vk::DescriptorSetAllocateInfo descriptorSetAllocateInfo;
    auto descriptorSetLayouts =
        std::vector<vk::DescriptorSetLayout>{pbrMaterial->GetPipeline()->GetMaterialDescriptorSetLayout()};
    descriptorSetAllocateInfo.setDescriptorPool(mVulkanContext->GetDescriptorPool())
        .setDescriptorSetCount(1)
        .setSetLayouts(descriptorSetLayouts);
    auto descriptorSets = mVulkanContext->GetDevice().allocateDescriptorSetsUnique(descriptorSetAllocateInfo);
    if (!descriptorSets.empty())
    {
        pbrMaterial->mMaterialDescriptorSet = std::move(descriptorSets[0]);
    }
    vk::BufferCreateInfo paramsBufferCreateInfo{};
    paramsBufferCreateInfo.setSize(sizeof(MPBRMaterialProperties))
        .setUsage(vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst)
        .setSharingMode(vk::SharingMode::eExclusive);
    VmaAllocationCreateInfo paramsAllocationCreateInfo{};
    paramsAllocationCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    paramsAllocationCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;
    if (vmaCreateBuffer(mVulkanContext->GetVmaAllocator(), &static_cast<VkBufferCreateInfo &>(paramsBufferCreateInfo),
                        &paramsAllocationCreateInfo, reinterpret_cast<VkBuffer *>(&pbrMaterial->mParamsUBO),
                        &pbrMaterial->mParamsUBOAllocation, &pbrMaterial->mParamsUBOAllocationInfo) != VK_SUCCESS)
    {
        LogError("Failed to create params UBO for PBR material");
        throw std::runtime_error("Failed to create params UBO for PBR material");
    }
    return pbrMaterial;
}
void MPBRMaterialManager::Write(std::shared_ptr<MPBRMaterial> material)
{
    // 写入材质参数
    memcpy(material->mParamsUBOAllocationInfo.pMappedData, &material->GetProperties(), sizeof(MPBRMaterialProperties));

    // 更新描述集
    std::vector<vk::WriteDescriptorSet> writeDescriptorSets;
    writeDescriptorSets.resize(material->mPipeline->GetMaterialDescriptorSetLayoutBindings().size());
    vk::DescriptorBufferInfo paramsBufferInfo;
    paramsBufferInfo.setBuffer(material->mParamsUBO).setOffset(0).setRange(sizeof(MPBRMaterialProperties));
    writeDescriptorSets[0]
        .setDstSet(material->GetMaterialDescriptorSet())
        .setDstBinding(0)
        .setDescriptorType(vk::DescriptorType::eUniformBuffer)
        .setBufferInfo(paramsBufferInfo)
        .setDescriptorCount(1);
    // albedo
    vk::DescriptorImageInfo albedoImageInfo;
    albedoImageInfo.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
        .setImageView(material->GetTextures().Albedo->GetImageView())
        .setSampler(material->GetTextures().Albedo->GetSampler());
    writeDescriptorSets[1]
        .setDstSet(material->GetMaterialDescriptorSet())
        .setDstBinding(1)
        .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
        .setImageInfo(albedoImageInfo)
        .setDescriptorCount(1);
    // normal
    vk::DescriptorImageInfo normalImageInfo;
    normalImageInfo.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
        .setImageView(material->GetTextures().Normal->GetImageView())
        .setSampler(material->GetTextures().Normal->GetSampler());
    writeDescriptorSets[2]
        .setDstSet(material->GetMaterialDescriptorSet())
        .setDstBinding(2)
        .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
        .setImageInfo(normalImageInfo)
        .setDescriptorCount(1);
    // arm
    vk::DescriptorImageInfo armImageInfo;
    armImageInfo.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
        .setImageView(material->GetTextures().ARM->GetImageView())
        .setSampler(material->GetTextures().ARM->GetSampler());
    writeDescriptorSets[3]
        .setDstSet(material->GetMaterialDescriptorSet())
        .setDstBinding(3)
        .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
        .setImageInfo(armImageInfo)
        .setDescriptorCount(1);
    // emissive
    vk::DescriptorImageInfo emissiveImageInfo;
    emissiveImageInfo.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
        .setImageView(material->GetTextures().Emissive->GetImageView())
        .setSampler(material->GetTextures().Emissive->GetSampler());
    writeDescriptorSets[4]
        .setDstSet(material->GetMaterialDescriptorSet())
        .setDstBinding(4)
        .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
        .setImageInfo(emissiveImageInfo)
        .setDescriptorCount(1);
    mVulkanContext->GetDevice().updateDescriptorSets(writeDescriptorSets, {});
}

void MPBRMaterialManager::CreateDefault()
{
    auto pbrMaterialSetting = MPBRMaterialSetting();
    auto pbrMaterial = Create(pbrMaterialSetting, "Default PBR Material");
    Remove(pbrMaterial->GetID());
    pbrMaterial->SetID(UUID{});
    mAssets[pbrMaterial->GetID()] = pbrMaterial;
}
} // namespace MEngine::Core::Manager