#include "ShaderBuilder.hpp"
#include "ShaderUtils.hpp"
#include "UUIDGenerator.hpp"
#include <fstream>

namespace MEngine::Core
{

void ShaderBuilder::Reset()
{
    mShaderModuleCreateInfo = vk::ShaderModuleCreateInfo();
    mCode.clear();
    mGLSLFilePath.clear();
    mGLSLSource.clear();
    mSPIRVFilePath.clear();
    mSPIRVCode.clear();
}
std::unique_ptr<Shader> ShaderBuilder::Build()
{
    if (mSPIRVCode.empty())
    {
        throw std::runtime_error("SPIR-V code is empty. Please set SPIR-V code or compile GLSL to SPIR-V.");
    }
    if (mName.empty())
    {
        throw std::runtime_error("Shader name is empty. Please set a valid name.");
    }
    mShaderModuleCreateInfo.setCode(mSPIRVCode);
    vk::UniqueShaderModule shaderModule = mVulkanContext->GetDevice().createShaderModuleUnique(mShaderModuleCreateInfo);
    if (!shaderModule)
    {
        throw std::runtime_error("Failed to create shader module.");
    }
    return std::make_unique<Shader>(UUIDGenerator::Instance().Create(), mName, std::move(shaderModule), mStage);
}
void ShaderBuilder::SetGLSLFile(const std::filesystem::path &path)
{
    mGLSLFilePath = path;
    // 读取shader文件
    std::fstream shaderFile(path, std::ios::in);
    if (!shaderFile.is_open())
    {
        throw std::runtime_error("Failed to open shader file: " + path.string());
    }
    std::string shaderSource((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
    mGLSLSource = shaderSource;
}
void ShaderBuilder::SetGLSL(const std::string &source)
{
    mGLSLSource = source;
}
void ShaderBuilder::SetSPRIVFile(const std::filesystem::path &path)
{
    mSPIRVFilePath = path;
    // 读取SPIR-V二进制文件
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open SPIR-V file: " + path.string());
    }
    mSPIRVCode = std::vector<uint32_t>((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}
void ShaderBuilder::SetSPIRV(const std::vector<uint32_t> &code)
{
    mSPIRVCode = code;
}
void ShaderBuilder::CompileGLSLToSPIRV()
{
    switch (mStage)
    {
    case vk::ShaderStageFlagBits::eVertex: {
        auto result = Utils::ShaderUtils::CompileShader(mGLSLSource, shaderc_shader_kind::shaderc_glsl_vertex_shader,
                                                        mGLSLFilePath.string());
        mSPIRVCode = std::vector<uint32_t>(result.cbegin(), result.cend());
        break;
    }
    case vk::ShaderStageFlagBits::eTessellationControl: {
        auto result = Utils::ShaderUtils::CompileShader(
            mGLSLSource, shaderc_shader_kind::shaderc_glsl_tess_control_shader, mGLSLFilePath.string());
        mSPIRVCode = std::vector<uint32_t>(result.cbegin(), result.cend());
        break;
    }
    case vk::ShaderStageFlagBits::eTessellationEvaluation: {
        auto result = Utils::ShaderUtils::CompileShader(
            mGLSLSource, shaderc_shader_kind::shaderc_glsl_tess_evaluation_shader, mGLSLFilePath.string());
        mSPIRVCode = std::vector<uint32_t>(result.cbegin(), result.cend());
        break;
    }
    case vk::ShaderStageFlagBits::eGeometry: {
        auto result = Utils::ShaderUtils::CompileShader(mGLSLSource, shaderc_shader_kind::shaderc_glsl_geometry_shader,
                                                        mGLSLFilePath.string());
        mSPIRVCode = std::vector<uint32_t>(result.cbegin(), result.cend());
        break;
    }
    case vk::ShaderStageFlagBits::eFragment: {
        auto result = Utils::ShaderUtils::CompileShader(mGLSLSource, shaderc_shader_kind::shaderc_glsl_fragment_shader,
                                                        mGLSLFilePath.string());
        mSPIRVCode = std::vector<uint32_t>(result.cbegin(), result.cend());
        break;
    }
    case vk::ShaderStageFlagBits::eCompute: {
        auto result = Utils::ShaderUtils::CompileShader(mGLSLSource, shaderc_shader_kind::shaderc_glsl_compute_shader,
                                                        mGLSLFilePath.string());
        mSPIRVCode = std::vector<uint32_t>(result.cbegin(), result.cend());
        break;
    }
    case vk::ShaderStageFlagBits::eAllGraphics:
    case vk::ShaderStageFlagBits::eAll:
    case vk::ShaderStageFlagBits::eRaygenKHR:
    case vk::ShaderStageFlagBits::eAnyHitKHR:
    case vk::ShaderStageFlagBits::eClosestHitKHR:
    case vk::ShaderStageFlagBits::eMissKHR:
    case vk::ShaderStageFlagBits::eIntersectionKHR:
    case vk::ShaderStageFlagBits::eCallableKHR:
    case vk::ShaderStageFlagBits::eTaskEXT:
    case vk::ShaderStageFlagBits::eMeshEXT:
    case vk::ShaderStageFlagBits::eSubpassShadingHUAWEI:
    case vk::ShaderStageFlagBits::eClusterCullingHUAWEI:
        break;
    }
}
void ShaderBuilder::SetName(const std::string &name)
{
    mName = name;
}
} // namespace MEngine::Core