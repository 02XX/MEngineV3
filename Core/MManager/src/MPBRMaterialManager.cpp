#include "MPBRMaterialManager.hpp"
#include "MPBRMaterial.hpp"
#include "MPipeline.hpp"
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
    auto pbrMaterial = std::make_shared<MPBRMaterial>(mUUIDGenerator->Create(), name, mVulkanContext, setting);
    return pbrMaterial;
}
void MPBRMaterialManager::CreateVulkanResources(std::shared_ptr<MPBRMaterial> pbrMaterial)
{
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
}
void MPBRMaterialManager::Write(std::shared_ptr<MPBRMaterial> material)
{
    material->mTextures.Albedo = mTextureManager->Get(material->mTextures.AlbedoID);
    material->mTextures.Normal = mTextureManager->Get(material->mTextures.NormalID);
    material->mTextures.ARM = mTextureManager->Get(material->mTextures.ARMID);
    material->mTextures.Emissive = mTextureManager->Get(material->mTextures.EmissiveID);
    // 写入材质参数
    memcpy(material->mParamsUBOAllocationInfo.pMappedData, &material->mProperties, sizeof(MPBRMaterialProperties));

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
}
std::shared_ptr<MPBRMaterial> MPBRMaterialManager::CreateDefaultMaterial()
{
    auto defaultMaterialSetting = MPBRMaterialSetting();
    auto defaultPBRMaterial = Create(defaultMaterialSetting, "Default PBR Material");
    defaultPBRMaterial->mPipelineName = PipelineType::ForwardOpaquePBR;
    defaultPBRMaterial->mPipeline = mPipelineManager->Get(defaultPBRMaterial->mPipelineName);
    defaultPBRMaterial->mTextures.AlbedoID = mTextureManager->GetDefaultTexture(DefaultTextureType::Albedo)->GetID();
    defaultPBRMaterial->mTextures.NormalID = mTextureManager->GetDefaultTexture(DefaultTextureType::Normal)->GetID();
    defaultPBRMaterial->mTextures.EmissiveID =
        mTextureManager->GetDefaultTexture(DefaultTextureType::Emissive)->GetID();
    defaultPBRMaterial->mTextures.ARMID = mTextureManager->GetDefaultTexture(DefaultTextureType::ARM)->GetID();
    defaultPBRMaterial->mTextures.Albedo = mTextureManager->Get(defaultPBRMaterial->mTextures.AlbedoID);
    defaultPBRMaterial->mTextures.Normal = mTextureManager->Get(defaultPBRMaterial->mTextures.NormalID);
    defaultPBRMaterial->mTextures.ARM = mTextureManager->Get(defaultPBRMaterial->mTextures.ARMID);
    defaultPBRMaterial->mTextures.Emissive = mTextureManager->Get(defaultPBRMaterial->mTextures.EmissiveID);
    CreateVulkanResources(defaultPBRMaterial);
    Write(defaultPBRMaterial);
    return defaultPBRMaterial;
}
} // namespace MEngine::Core::Manager