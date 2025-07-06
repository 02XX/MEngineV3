#pragma once
#include "IMPipelineManager.hpp"
#include "MManager.hpp"
#include "MPipeline.hpp"

#include "RenderPassManager.hpp"
#include <memory>
#include <vulkan/vulkan_handles.hpp>

using namespace MEngine::Core::Asset;
namespace MEngine::Core::Manager
{

class MPipelineManager final : public MManager<MPipeline, MPipelineSetting>, public IMPipelineManager
{
  private:
    std::shared_ptr<RenderPassManager> mRenderPassManager;

  public:
    MPipelineManager(std::shared_ptr<VulkanContext> vulkanContext, std::shared_ptr<IUUIDGenerator> uuidGenerator,
                     std::shared_ptr<RenderPassManager> renderPassManager)
        : MManager(vulkanContext, uuidGenerator), mRenderPassManager(renderPassManager)
    {
    }
    ~MPipelineManager() override = default;
    std::shared_ptr<MPipeline> Create(const MPipelineSetting &setting) override;
    void Update(std::shared_ptr<MPipeline> pipeline) override
    {
    }
    vk::UniqueShaderModule CreateShaderModule(const std::filesystem::path &shaderPath) const override;
};
} // namespace MEngine::Core::Manager