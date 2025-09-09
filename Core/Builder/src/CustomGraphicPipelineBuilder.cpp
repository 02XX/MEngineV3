
#include "CustomGraphicPipelineBuilder.hpp"

namespace MEngine::Core
{
void CustomGraphicPipelineBuilder::Reset()
{
    GraphicPipelineBuilder::Reset();
    mCustomShaders.clear();
}
void CustomGraphicPipelineBuilder::SetName(const std::string &name)
{
    mGraphicPipeline->mName = name;
}
void CustomGraphicPipelineBuilder::SetShader()
{
    for (const auto &[stage, shaderName] : mCustomShaders)
    {
        auto shader = mShaderManager->GetByName(shaderName);
        if (!shader)
            throw std::runtime_error("Shader " + shaderName + " for Custom Pipeline not found");
        mGraphicPipeline->mShaderStages.push_back(
            vk::PipelineShaderStageCreateInfo{}.setStage(stage).setModule(shader->GetShaderModule()).setPName("main"));
    }
}
void CustomGraphicPipelineBuilder::SetCustomShader(vk::ShaderStageFlagBits stage, const std::string &shaderName)
{
    mCustomShaders[stage] = shaderName;
}

} // namespace MEngine::Core