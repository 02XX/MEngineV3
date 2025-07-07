#include "ResourceManager.hpp"
#include "Logger.hpp"
#include "MTexture.hpp"

namespace MEngine::Resource
{
void ResourceManager::UpdateAsset(const UUID &id)
{
}

void ResourceManager::DeleteAsset(const UUID &id)
{
}
void ResourceManager::CreateDefaultAssets()
{
    // Texture
    auto defaultTextureSetting = MTextureSetting();
    defaultTextureSetting.width = 1;
    defaultTextureSetting.height = 1;
    defaultTextureSetting.format = vk::Format::eR8G8B8A8Unorm;
    auto defaultTexture = CreateAsset<MTexture>(defaultTextureSetting);
    defaultTexture->SetID(UUID{});
    auto whitePixel = std::vector<uint8_t>(4, 255);
    mTextureManager->Write(defaultTexture, whitePixel,
                           TextureSize{defaultTextureSetting.width, defaultTextureSetting.height, 4});
    mAssets[typeid(MTexture)][defaultTexture->GetID()] = defaultTexture;
    // Pipeline
    auto defaultPipelineSetting = MPipelineSetting();
    defaultPipelineSetting.VertexShaderPath = "Assets/Shaders/Default.vert";
    defaultPipelineSetting.FragmentShaderPath = "Assets/Shaders/Default.frag";
    auto defaultPipeline = CreateAsset<MPipeline>(defaultPipelineSetting);
    defaultPipeline->SetID(UUID{});
    mAssets[typeid(MPipeline)][defaultPipeline->GetID()] = defaultPipeline;
    // PBR Material
    auto defaultPBRMaterialSetting = MPBRMaterialSetting();
    auto defaultPBRMaterial = CreateAsset<MPBRMaterial>(defaultPBRMaterialSetting);
    defaultPBRMaterial->SetID(UUID{});
    defaultPBRMaterial->GetProperties().Textures.Albedo = defaultTexture;
    defaultPBRMaterial->GetProperties().Textures.Normal = defaultTexture;
    defaultPBRMaterial->GetProperties().Textures.ARM = defaultTexture;
    defaultPBRMaterial->GetProperties().Textures.Emissive = defaultTexture;
    mAssets[typeid(MPBRMaterial)][defaultPBRMaterial->GetID()] = defaultPBRMaterial;
    LogDebug("Created default assets: Default Texture, Default Pipeline, Default PBR Material");
}
} // namespace MEngine::Resource