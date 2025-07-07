#pragma once
#include <shaderc/shaderc.h>
#include <shaderc/shaderc.hpp>

namespace MEngine::Core::Utils
{
class ShaderUtils
{
  public:
    static shaderc::Compiler &GetCompiler();
    static shaderc::CompileOptions &GetCompileOptions();
    static shaderc::SpvCompilationResult CompileShader(const std::string &source, shaderc_shader_kind kind,
                                                       const std::string &name = "shader");
    static shaderc_shader_kind GetShaderKindFromExtension(const std::string &extension);
};
} // namespace MEngine::Core::Utils
