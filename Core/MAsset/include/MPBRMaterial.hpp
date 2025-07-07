#pragma once
#include "MMaterial.hpp"
#include "MTexture.hpp"
#include <memory>
namespace MEngine::Core::Asset
{
class MPBRMaterialSetting final : public MMaterialSetting
{
};
struct MPBRTextures
{
    std::shared_ptr<MTexture> Albedo{};
    std::shared_ptr<MTexture> Normal{};
    std::shared_ptr<MTexture> ARM{};
    std::shared_ptr<MTexture> Emissive{};
};
struct MPBRMaterialProperties
{
    MPBRTextures Textures;
    float Metallic = 0.0f;
    float Roughness = 1.0f;
    float AO = 1.0f;
    float EmissiveIntensity = 1.0f;
};
class MPBRMaterial final : public MMaterial
{
  private:
    MPBRMaterialProperties mProperties;
    MPBRTextures mTextures;
    vk::UniqueDescriptorSetLayout mDescriptorSetLayout;

  public:
    MPBRMaterial(const UUID &id, const MPBRMaterialSetting &setting)
        : MMaterial(id, setting), mProperties(), mTextures()
    {
    }
    inline MPBRMaterialProperties &GetProperties()
    {
        return mProperties;
    }
    inline const MPBRMaterialProperties &GetProperties() const
    {
        return mProperties;
    }
    inline MPBRTextures &GetTextures()
    {
        return mTextures;
    }
    inline const MPBRTextures &GetTextures() const
    {
        return mTextures;
    }
};
} // namespace MEngine::Core::Asset
