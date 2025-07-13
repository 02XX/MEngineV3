#pragma once
#include "IMPipelineManager.hpp"
#include "Logger.hpp"
#include "MManager.hpp"
#include "MPipeline.hpp"

#include "RenderPassManager.hpp"
#include <memory>

#include <unordered_map>
#include <vector>
#include <vulkan/vulkan_handles.hpp>

using namespace MEngine::Core::Asset;
namespace MEngine::Core::Manager
{

class MPipelineManager final : public MManager<MPipeline>, public IMPipelineManager
{

  private:
    std::shared_ptr<RenderPassManager> mRenderPassManager;
    std::unordered_map<std::string, std::shared_ptr<MPipeline>> mPipelines;
    std::vector<vk::DescriptorSetLayoutBinding> mGlobalDescriptorSetLayoutBindings{
        // set:0
        // Binding: 0 VP (View Projection Matrix)
        vk::DescriptorSetLayoutBinding{0, vk::DescriptorType::eUniformBuffer, 1,
                                       vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment},
        // Binding: 1 Light
        vk::DescriptorSetLayoutBinding{1, vk::DescriptorType::eUniformBuffer, 1,
                                       vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment},
        // Binding: 2 Environment Map
        vk::DescriptorSetLayoutBinding{2, vk::DescriptorType::eCombinedImageSampler, 1,
                                       vk::ShaderStageFlagBits::eFragment},
        // Binding: 3 Irradiance Map
        vk::DescriptorSetLayoutBinding{3, vk::DescriptorType::eCombinedImageSampler, 1,
                                       vk::ShaderStageFlagBits::eFragment},
        // Binding: 4 BRDF LUT
        vk::DescriptorSetLayoutBinding{4, vk::DescriptorType::eCombinedImageSampler, 1,
                                       vk::ShaderStageFlagBits::eFragment},
    };
    std::unordered_map<std::string, std::vector<vk::DescriptorSetLayoutBinding>> mDescriptorSetLayoutBindings{};

    vk::UniqueDescriptorSetLayout mGlobalDescriptorSetLayout;
    std::unordered_map<std::string, vk::DescriptorSetLayout> mMaterialDescriptorSetLayouts;

  public:
    MPipelineManager(std::shared_ptr<VulkanContext> vulkanContext, std::shared_ptr<IUUIDGenerator> uuidGenerator,
                     std::shared_ptr<RenderPassManager> renderPassManager);
    ~MPipelineManager() override = default;
    std::shared_ptr<MPipeline> Create(const std::string &name, const MPipelineSetting &setting) override;
    std::shared_ptr<MPipeline> GetByName(const std::string &name) const override;
    void Update(std::shared_ptr<MPipeline> pipeline) override;
    vk::UniqueShaderModule CreateShaderModule(const std::filesystem::path &shaderPath) const override;
    void Remove(const UUID &id) override;
    void RemoveByName(const std::string &name) override;
    void CreateDefault() override;
    virtual void CreateVulkanResources(std::shared_ptr<MPipeline> asset) override;
    inline std::vector<vk::DescriptorSetLayoutBinding> GetGlobalDescriptorSetLayoutBindings() const override
    {
        return mGlobalDescriptorSetLayoutBindings;
    }
    inline std::vector<vk::DescriptorSetLayoutBinding> GetMaterialDescriptorSetLayoutBindings(
        const std::string &pipelineType) const override
    {
        if (mDescriptorSetLayoutBindings.contains(pipelineType))
        {
            return mDescriptorSetLayoutBindings.at(pipelineType);
        }
        LogWarn("Pipeline type {} does not exist in descriptor set layout bindings", pipelineType);
        return {};
    }
    inline vk::DescriptorSetLayout GetGlobalDescriptorSetLayout() const override
    {
        return mGlobalDescriptorSetLayout.get();
    }
    inline vk::DescriptorSetLayout GetMaterialDescriptorSetLayout(const std::string &pipelineType) const override
    {
        if (mMaterialDescriptorSetLayouts.contains(pipelineType))
        {
            return mMaterialDescriptorSetLayouts.at(pipelineType);
        }
        LogWarn("Pipeline type {} does not exist in material descriptor set layouts", pipelineType);
        return {};
    }
};
} // namespace MEngine::Core::Manager