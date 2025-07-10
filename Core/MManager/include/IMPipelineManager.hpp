#pragma once
#include "IMManager.hpp"
#include "MPipeline.hpp"

#include <vulkan/vulkan_handles.hpp>
using namespace MEngine::Core::Asset;
namespace MEngine::Core::Manager
{
class IMPipelineManager : public virtual IMManager<MPipeline>
{
  public:
    ~IMPipelineManager() override = default;
    virtual std::shared_ptr<MPipeline> Create(const std::string &name, const MPipelineSetting &setting) = 0;
    virtual vk::UniqueShaderModule CreateShaderModule(const std::filesystem::path &shaderPath) const = 0;
    virtual std::shared_ptr<MPipeline> GetByName(const std::string &name) const = 0;
    virtual void RemoveByName(const std::string &name) = 0;
    virtual std::vector<vk::DescriptorSetLayoutBinding> GetGlobalDescriptorSetLayoutBindings() const = 0;
    virtual std::vector<vk::DescriptorSetLayoutBinding> GetMaterialDescriptorSetLayoutBindings(
        const std::string &pipelineType) const = 0;
    virtual vk::DescriptorSetLayout GetGlobalDescriptorSetLayout() const = 0;
    virtual vk::DescriptorSetLayout GetMaterialDescriptorSetLayout(const std::string &pipelineType) const = 0;
};
} // namespace MEngine::Core::Manager