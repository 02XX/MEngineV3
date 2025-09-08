#pragma once
#include "Asset.hpp"
#include "UUID.hpp"
#include <vulkan/vulkan.hpp>

namespace MEngine::Core
{
class Shader final : public Asset
{
  private:
    vk::UniqueShaderModule mShaderModule{nullptr};
    // TODO: 冗余下个版本是否删除？
    vk::ShaderStageFlagBits mStage{vk::ShaderStageFlagBits::eVertex};

  protected:
    Shader() : Asset(UUID{}, "Unnamed")
    {
        mType = AssetType::Shader;
    }

  public:
    Shader(const UUID &id, const std::string &name, vk::UniqueShaderModule shaderModule, vk::ShaderStageFlagBits stage)
        : Asset(id, name), mShaderModule(std::move(shaderModule)), mStage(stage)
    {
        mType = AssetType::Shader;
    }
    ~Shader() override = default;
    inline vk::ShaderModule GetShaderModule() const
    {
        return mShaderModule.get();
    }
    inline vk::ShaderStageFlagBits GetStage() const
    {
        return mStage;
    }
};
} // namespace MEngine::Core