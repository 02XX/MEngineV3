#pragma once
#include "MMaterial.hpp"
#include "MPipeline.hpp"
#include "MTexture.hpp"
#include "Math.hpp"
#include "VulkanContext.hpp"
#include <memory>
#include <span>
#include <vulkan/vulkan_handles.hpp>

namespace MEngine::Core::Asset
{
class MPBRMaterialSetting final : public MMaterialSetting
{
  public:
    ~MPBRMaterialSetting() override = default;
};
struct MPBRTextures
{
    UUID AlbedoID{};
    UUID NormalID{};
    UUID ARMID{}; // Ambient Occlusion, Roughness, Metallic
    UUID EmissiveID{};
    // 导航属性
    std::shared_ptr<MTexture> Albedo{};
    std::shared_ptr<MTexture> Normal{};
    std::shared_ptr<MTexture> ARM{};
    std::shared_ptr<MTexture> Emissive{};
};
struct MPBRMaterialProperties
{
    alignas(16) glm::vec3 Albedo = glm::vec3(1.0f, 1.0f, 1.0f);
    alignas(16) glm::vec3 Normal = glm::vec3(1.0f, 1.0f, 1.0f);
    float Metallic = 0.0f;
    float Roughness = 1.0f;
    float AO = 1.0f;
    float EmissiveIntensity = 1.0f;
};

class MPBRMaterial final : public MMaterial
{
    friend class nlohmann::adl_serializer<MPBRMaterial>;
    friend class MEngine::Core::Manager::MPBRMaterialManager;

  private:
    std::shared_ptr<VulkanContext> mVulkanContext{};
    MPBRMaterialProperties mProperties;
    MPBRTextures mTextures;
    MPBRMaterialSetting mSetting;
    vk::Buffer mParamsUBO;
    VmaAllocation mParamsUBOAllocation;
    VmaAllocationInfo mParamsUBOAllocationInfo;

  public:
    MPBRMaterial(const UUID &id, const std::string &name, std::shared_ptr<VulkanContext> vulkanContext,
                 const std::string &pipelineName, const MPBRMaterialProperties &properties,
                 const MPBRTextures &textures, const MPBRMaterialSetting &setting)
        : MMaterial(id, name, pipelineName, setting), mSetting(setting), mVulkanContext(vulkanContext),
          mProperties(properties), mTextures(textures)
    {
        mMaterialType = MMaterialType::PBR;
    }
    ~MPBRMaterial() override
    {
        if (mParamsUBO)
        {
            vmaDestroyBuffer(mVulkanContext->GetVmaAllocator(), mParamsUBO, mParamsUBOAllocation);
        }
    }
    inline const MPBRMaterialProperties &GetProperties() const
    {
        return mProperties;
    }
    inline const MPBRTextures &GetTextures() const
    {
        return mTextures;
    }

    inline void SetAlbedo(const glm::vec3 &albedo)
    {
        mProperties.Albedo = albedo;
    }
    inline void SetNormal(const glm::vec3 &normal)
    {
        mProperties.Normal = normal;
    }
    inline void SetMetallic(float metallic)
    {
        mProperties.Metallic = metallic;
    }
    inline void SetRoughness(float roughness)
    {
        mProperties.Roughness = roughness;
    }
    inline void SetAO(float ao)
    {
        mProperties.AO = ao;
    }
    inline void SetEmissiveIntensity(float emissiveIntensity)
    {
        mProperties.EmissiveIntensity = emissiveIntensity;
    }
    inline void SetAlbedoTextureID(const UUID &id)
    {
        mTextures.AlbedoID = id;
    }
    inline void SetNormalTextureID(const UUID &id)
    {
        mTextures.NormalID = id;
    }
    inline void SetARMTextureID(const UUID &id)
    {
        mTextures.ARMID = id;
    }
    inline void SetEmissiveTextureID(const UUID &id)
    {
        mTextures.EmissiveID = id;
    }
    inline void SetProperties(const MPBRMaterialProperties &properties)
    {
        mProperties = properties;
    }
    inline void SetTextures(const MPBRTextures &textures)
    {
        mTextures = textures;
    }
    inline std::span<const vk::DescriptorSetLayoutBinding> GetDescriptorSetLayoutBindings() const override
    {
        static std::vector<vk::DescriptorSetLayoutBinding> mPBRDescriptorSetLayoutBindings{
            // set:1
            // Binding: 0 Parameters
            vk::DescriptorSetLayoutBinding{0, vk::DescriptorType::eUniformBuffer, 1,
                                           vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment},
            // Binding: 1 Albedo
            vk::DescriptorSetLayoutBinding{1, vk::DescriptorType::eCombinedImageSampler, 1,
                                           vk::ShaderStageFlagBits::eFragment},
            // Binding: 2 Normal Map
            vk::DescriptorSetLayoutBinding{2, vk::DescriptorType::eCombinedImageSampler, 1,
                                           vk::ShaderStageFlagBits::eFragment},
            // Binding: 3 ARM (Ambient Occlusion, Roughness, Metallic)
            vk::DescriptorSetLayoutBinding{3, vk::DescriptorType::eCombinedImageSampler, 1,
                                           vk::ShaderStageFlagBits::eFragment},
            // Binding: 4 Emissive
            vk::DescriptorSetLayoutBinding{4, vk::DescriptorType::eCombinedImageSampler, 1,
                                           vk::ShaderStageFlagBits::eFragment}};

        return mPBRDescriptorSetLayoutBindings;
    }
};
} // namespace MEngine::Core::Asset
