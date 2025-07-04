#pragma once
#include "MAssetSetting.hpp"
#include <cstdint>
#include <vulkan/vulkan.hpp>
namespace MEngine::Core::Asset
{

class MTextureSetting final : public MAssetSetting
{
  public:
    uint32_t width = 1;
    uint32_t height = 1;
    vk::ImageViewType ImageType = vk::ImageViewType::e2D;
    uint32_t mipmapLevels = 1;
    uint32_t arrayLayers = 1;
    vk::Format format = vk::Format::eR8G8B8A8Srgb;
    vk::SampleCountFlagBits sampleCount = vk::SampleCountFlagBits::e1;
    bool isShaderResource = true;
    bool isRenderTarget = false;
    bool isUAV = false;
    bool isShadingRateSurface = false;
    bool isTypeless = false;
    bool isTiled = false;
    vk::SamplerAddressMode addressModeU = vk::SamplerAddressMode::eRepeat;
    vk::SamplerAddressMode addressModeV = vk::SamplerAddressMode::eRepeat;
    vk::SamplerAddressMode addressModeW = vk::SamplerAddressMode::eRepeat;
    vk::Filter minFilter = vk::Filter::eLinear;
    vk::Filter magFilter = vk::Filter::eLinear;
    vk::SamplerMipmapMode mipmapMode = vk::SamplerMipmapMode::eLinear;
    float mipLodBias = 0.0f;
    float minLod = 0.0f;
    float maxLod = 0.0f;
    vk::BorderColor borderColor = vk::BorderColor::eFloatOpaqueBlack;
    bool compareEnable = false;
    vk::CompareOp compareOp = vk::CompareOp::eAlways;
    bool anisotropyEnable = false;
    float maxAnisotropy = 1.0f;
    vk::Bool32 unnormalizedCoordinates = vk::False;

  public:
    ~MTextureSetting() override = default;
};
} // namespace MEngine::Core::Asset