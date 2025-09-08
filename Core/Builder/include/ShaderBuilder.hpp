#pragma once

#include "IShaderBuilder.hpp"
#include "VulkanContext.hpp"
#include <filesystem>
#include <memory>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>

namespace MEngine::Core
{
class ShaderBuilder final : public virtual IShaderBuilder
{
  protected:
    std::shared_ptr<VulkanContext> mVulkanContext;

    vk::ShaderModuleCreateInfo mShaderModuleCreateInfo{};
    std::vector<uint32_t> mCode{};
    vk::ShaderStageFlagBits mStage{vk::ShaderStageFlagBits::eVertex};

    std::filesystem::path mGLSLFilePath{};
    std::string mGLSLSource{};
    std::filesystem::path mSPIRVFilePath{};
    std::vector<uint32_t> mSPIRVCode{};

    std::string mName{};

  public:
    ShaderBuilder(std::shared_ptr<VulkanContext> vulkanContext) : mVulkanContext(vulkanContext)
    {
    }
    ~ShaderBuilder() override = default;
    std::unique_ptr<Shader> Build() override;
    void Reset() override;
    void SetName(const std::string &name) override;
    void SetGLSLFile(const std::filesystem::path &path) override;
    void SetGLSL(const std::string &source) override;
    void SetSPRIVFile(const std::filesystem::path &path) override;
    void SetSPIRV(const std::vector<uint32_t> &code) override;
    void CompileGLSLToSPIRV() override;
    inline void SetStage(vk::ShaderStageFlagBits stage) override
    {
        mStage = stage;
    }
};
} // namespace MEngine::Core