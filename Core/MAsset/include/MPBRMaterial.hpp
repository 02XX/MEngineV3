#pragma once
#include "MMaterial.hpp"
#include "MTexture.hpp"
namespace MEngine::Core::Asset
{
class MPBRMaterialSetting final : public MMaterialSetting
{
};
struct MPBRTextures
{
    UUID AlbedoID{};
    UUID NormalID{};
    UUID ARMID{};
    UUID EmissiveID{};
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
