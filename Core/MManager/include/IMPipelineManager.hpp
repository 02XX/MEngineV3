#pragma once
#include "IMManager.hpp"
#include "MPipeline.hpp"

#include <vulkan/vulkan_handles.hpp>
using namespace MEngine::Core::Asset;
namespace MEngine::Core::Manager
{
class IMPipelineManager : public virtual IMManager<MPipeline, MPipelineSetting>
{
  public:
    ~IMPipelineManager() override = default;
    virtual vk::UniqueShaderModule CreateShaderModule(const std::filesystem::path &shaderPath) const = 0;
    virtual std::shared_ptr<MPipeline> Get(const std::string &name) const = 0;
    virtual void Remove(const std::string &name) = 0;
    virtual std::vector<vk::DescriptorSetLayoutBinding> GetGlobalDescriptorSetLayoutBindings() const = 0;
    virtual std::vector<vk::DescriptorSetLayoutBinding> GetMaterialDescriptorSetLayoutBindings(
        const std::string &pipelineType) const = 0;
    virtual vk::DescriptorSetLayout GetGlobalDescriptorSetLayout() const = 0;
    virtual vk::DescriptorSetLayout GetMaterialDescriptorSetLayout(const std::string &pipelineType) const = 0;
};
} // namespace MEngine::Core::Manager