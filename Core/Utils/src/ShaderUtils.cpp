#include "ShaderUtils.hpp"
#include <stdexcept>

namespace MEngine::Core::Utils
{

shaderc::Compiler &ShaderUtils::GetCompiler()
{
    static shaderc::Compiler compiler;
    return compiler;
}
shaderc::CompileOptions &ShaderUtils::GetCompileOptions()
{
    static shaderc::CompileOptions options;
    options.SetOptimizationLevel(shaderc_optimization_level::shaderc_optimization_level_performance);
    options.SetGenerateDebugInfo();
    options.SetHlslFunctionality1(true);
    options.SetHlsl16BitTypes(false);
    options.SetVulkanRulesRelaxed(true);
    options.SetInvertY(false);
    options.SetNanClamp(true);
    return options;
}
shaderc::SpvCompilationResult ShaderUtils::CompileShader(const std::string &source, shaderc_shader_kind kind,
                                                         const std::string &name)
{
    shaderc::Compiler &compiler = GetCompiler();
    shaderc::CompileOptions &options = GetCompileOptions();

    shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(source, kind, name.c_str(), options);
    if (result.GetCompilationStatus() != shaderc_compilation_status_success)
    {
        throw std::runtime_error("Shader compilation failed: " + result.GetErrorMessage());
    }
    return result;
}
shaderc_shader_kind ShaderUtils::GetShaderKindFromExtension(const std::string &extension)
{
    if (extension == ".vert")
        return shaderc_shader_kind::shaderc_glsl_vertex_shader;
    else if (extension == ".frag")
        return shaderc_shader_kind::shaderc_glsl_fragment_shader;
    else if (extension == ".comp")
        return shaderc_shader_kind::shaderc_glsl_compute_shader;
    else if (extension == ".geom")
        return shaderc_shader_kind::shaderc_glsl_geometry_shader;
    else if (extension == ".tesc")
        return shaderc_shader_kind::shaderc_glsl_tess_control_shader;
    else if (extension == ".tese")
        return shaderc_shader_kind::shaderc_glsl_tess_evaluation_shader;
    else
        throw std::invalid_argument("Unsupported shader extension: " + extension);
}
} // namespace MEngine::Core::Utils
