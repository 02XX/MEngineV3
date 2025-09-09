#pragma once
#include "GraphicPipelineBuilder.hpp"
#include "ICustomGraphicPipelineBuilder.hpp"
#include <vulkan/vulkan.hpp>

namespace MEngine::Core
{
class CustomGraphicPipelineBuilder : public GraphicPipelineBuilder, public virtual ICustomGraphicPipelineBuilder
{
  private:
    std::unordered_map<vk::ShaderStageFlagBits, std::string> mCustomShaders{};

  public:
    CustomGraphicPipelineBuilder(std::shared_ptr<VulkanContext> vulkanContext,
                                 std::shared_ptr<IPipelineLayoutManager> pipelineLayoutManager,
                                 std::shared_ptr<IShaderManager> shaderManager,
                                 std::shared_ptr<RenderPassManager> renderPassManager)
        : GraphicPipelineBuilder(vulkanContext, pipelineLayoutManager, shaderManager, renderPassManager)
    {
    }
    ~CustomGraphicPipelineBuilder() override = default;
    virtual void SetName(const std::string &name) override;
    virtual void Reset() override;
    virtual void SetShader() override;
    virtual void SetCustomShader(vk::ShaderStageFlagBits stage, const std::string &shaderName) override;
};
} // namespace MEngine::Core