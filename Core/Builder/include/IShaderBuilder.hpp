#pragma once
#include "IBuilder.hpp"
#include "Shader.hpp"
#include <filesystem>
#include <vulkan/vulkan.hpp>

namespace MEngine::Core
{
class IShaderBuilder : public virtual IBuilder<Shader>
{
  public:
    ~IShaderBuilder() override = default;
    virtual void SetName(const std::string &name) = 0;
    virtual void SetGLSLFile(const std::filesystem::path &path) = 0;
    virtual void SetGLSL(const std::string &source) = 0;
    virtual void SetSPRIVFile(const std::filesystem::path &path) = 0;
    virtual void SetSPIRV(const std::vector<uint32_t> &code) = 0;
    virtual void CompileGLSLToSPIRV() = 0;
    virtual void SetStage(vk::ShaderStageFlagBits stage) = 0;
};
} // namespace MEngine::Core