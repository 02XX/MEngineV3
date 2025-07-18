#include "MPBRMaterialManager.hpp"
#include "MPBRMaterial.hpp"
#include "MPipeline.hpp"
#include "VMA.hpp"
#include <cstring>
#include <memory>
#include <vector>
namespace MEngine::Core::Manager
{
std::shared_ptr<MPBRMaterial> MPBRMaterialManager::Create(const std::string &name, const std::string &pipelineName,
                                                          const MPBRMaterialProperties &properties,
                                                          const MPBRTextures &textures,
                                                          const MPBRMaterialSetting &setting)
{
    auto pbrMaterial = std::make_shared<MPBRMaterial>(mUUIDGenerator->Create(), name, mVulkanContext, pipelineName,
                                                      properties, textures, setting);
    Update(pbrMaterial);
    mAssets[pbrMaterial->mID] = pbrMaterial;
    return pbrMaterial;
}
void MPBRMaterialManager::Update(std::shared_ptr<MPBRMaterial> pbrMaterial)
{
    // 设置导航属性
    pbrMaterial->mPipeline = mPipelineManager->GetByName(pbrMaterial->mPipelineName);
    pbrMaterial->mTextures.Albedo = mTextureManager->Get(pbrMaterial->mTextures.AlbedoID);
    pbrMaterial->mTextures.Normal = mTextureManager->Get(pbrMaterial->mTextures.NormalID);
    pbrMaterial->mTextures.ARM = mTextureManager->Get(pbrMaterial->mTextures.ARMID);
    pbrMaterial->mTextures.Emissive = mTextureManager->Get(pbrMaterial->mTextures.EmissiveID);
    mAssets[pbrMaterial->GetID()] = pbrMaterial;
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
    vk::DescriptorImageInfo albedoImageInfo{};
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
    vk::DescriptorImageInfo normalImageInfo{};
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
    vk::DescriptorImageInfo armImageInfo{};
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
    vk::DescriptorImageInfo emissiveImageInfo{};
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
std::shared_ptr<MPBRMaterial> MPBRMaterialManager::CreateDefaultForwardOpaquePBRMaterial()
{
    auto defaultMaterialSetting = MPBRMaterialSetting();
    auto defaultMaterialProperties = MPBRMaterialProperties{};
    auto defaultMaterialTextures = MPBRTextures{};
    defaultMaterialTextures.AlbedoID = mTextureManager->GetDefaultTexture(DefaultTextureType::Albedo)->GetID();
    defaultMaterialTextures.NormalID = mTextureManager->GetDefaultTexture(DefaultTextureType::Normal)->GetID();
    defaultMaterialTextures.ARMID = mTextureManager->GetDefaultTexture(DefaultTextureType::ARM)->GetID();
    defaultMaterialTextures.EmissiveID = mTextureManager->GetDefaultTexture(DefaultTextureType::Emissive)->GetID();
    auto defaultPBRMaterial = Create("Default PBR Material", PipelineType::ForwardOpaquePBR, defaultMaterialProperties,
                                     defaultMaterialTextures, defaultMaterialSetting);
    CreateVulkanResources(defaultPBRMaterial);
    Write(defaultPBRMaterial);
    return defaultPBRMaterial;
}
std::shared_ptr<MPBRMaterial> MPBRMaterialManager::CreateLightMaterial()
{
    auto lightMaterial = Create("Light Material", PipelineType::Lighting, MPBRMaterialProperties{}, MPBRTextures{},
                                MPBRMaterialSetting{});
    CreateVulkanResources(lightMaterial);
    Write(lightMaterial);
    return lightMaterial;
}
} // namespace MEngine::Core::Manager