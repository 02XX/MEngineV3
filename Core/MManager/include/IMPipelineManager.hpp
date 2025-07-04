#pragma once
#include "IMManager.hpp"
#include "MPipeline.hpp"
#include "MPipelineSetting.hpp"
#include <vulkan/vulkan_handles.hpp>
using namespace MEngine::Core::Asset;
namespace MEngine::Core::Manager
{
class IMPipelineManager : public IMManager<MPipeline, MPipelineSetting>
{
  public:
    ~IMPipelineManager() override = default;
    virtual vk::UniqueShaderModule CreateShaderModule(const std::filesystem::path &shaderPath) const = 0;
 
};
} // namespace MEngine::Core::Manager