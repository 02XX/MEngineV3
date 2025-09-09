#pragma once
#include "Asset.hpp"
#include "UUID.hpp"
#include <filesystem>
#include <vulkan/vulkan.hpp>

namespace MEngine::Core
{
class Shader final : public Asset
{
    friend class ShaderBuilder;

  private:
    vk::UniqueShaderModule mShaderModule{nullptr};

  protected:
    vk::ShaderModuleCreateInfo mShaderModuleCreateInfo{};
    std::vector<uint32_t> mCode{};
    vk::ShaderStageFlagBits mStage{vk::ShaderStageFlagBits::eVertex};

    std::filesystem::path mGLSLFilePath{};
    std::string mGLSLSource{};
    std::filesystem::path mSPIRVFilePath{};
    std::vector<uint32_t> mSPIRVCode{};

  protected:
    Shader() : Asset()
    {
        mType = AssetType::Shader;
    }

  public:
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