#include "MPipelineManager.hpp"
#include "Logger.hpp"
#include "MPBRMaterial.hpp"
#include "ShaderUtils.hpp"
#include "VMA.hpp"
#include "Vertex.hpp"
#include <cstring>
#include <fstream>
#include <memory>
#include <vector>

namespace MEngine::Core::Manager
{

MPipelineManager::MPipelineManager(std::shared_ptr<VulkanContext> vulkanContext,
                                   std::shared_ptr<IUUIDGenerator> uuidGenerator,
                                   std::shared_ptr<RenderPassManager> renderPassManager)
    : MManager(vulkanContext, uuidGenerator), mRenderPassManager(renderPassManager)
{
    vk::DescriptorSetLayoutCreateInfo globalDescriptorSetLayoutCreateInfo;
    globalDescriptorSetLayoutCreateInfo.setBindings(mGlobalDescriptorSetLayoutBindings)
        .setFlags(vk::DescriptorSetLayoutCreateFlagBits::eUpdateAfterBindPool);
    auto globalDescriptorSetLayout =
        mVulkanContext->GetDevice().createDescriptorSetLayoutUnique(globalDescriptorSetLayoutCreateInfo);
    if (!globalDescriptorSetLayout)
    {
        LogError("Failed to create descriptor set layout");
        throw std::runtime_error("Failed to create descriptor set layout");
    }
    mGlobalDescriptorSetLayout = std::move(globalDescriptorSetLayout);
    CreateDefault();
}
std::shared_ptr<MPipeline> MPipelineManager::Create(const std::string &name, const MPipelineSetting &setting)
{
    auto pipeline = std::make_shared<MPipeline>(mUUIDGenerator->Create(), name, mVulkanContext, setting);
    mAssets[pipeline->mID] = pipeline;
    mPipelines[pipeline->mName] = pipeline;
    mMaterialDescriptorSetLayouts[pipeline->GetName()] = pipeline->mMaterialDescriptorSetLayouts.get();
    LogDebug("Create pipeline: {}", pipeline->GetName());
    return pipeline;
}
void MPipelineManager::CreateVulkanResources(std::shared_ptr<MPipeline> pipeline)
{
    // 创建着色器模块
    // vertex shader
    pipeline->mVertexShaderModule = CreateShaderModule(pipeline->GetSetting().VertexShaderPath);
    // fragment shader
    pipeline->mFragmentShaderModule = CreateShaderModule(pipeline->mSetting.FragmentShaderPath);
    LogDebug("Shader modules created successfully: {} and {}", pipeline->mSetting.VertexShaderPath.string(),
             pipeline->mSetting.FragmentShaderPath.string());
    // 创建pipeline layout
    vk::DescriptorSetLayoutCreateInfo materialDescriptorSetLayoutCreateInfo;
    materialDescriptorSetLayoutCreateInfo.setBindings(pipeline->mSetting.MaterialDescriptorSetLayoutBindings)
        .setFlags(vk::DescriptorSetLayoutCreateFlagBits::eUpdateAfterBindPool);
    auto materialDescriptorSetLayout =
        mVulkanContext->GetDevice().createDescriptorSetLayoutUnique(materialDescriptorSetLayoutCreateInfo);
    if (!materialDescriptorSetLayout)
    {
        LogError("Failed to create material descriptor set layout");
        throw std::runtime_error("Failed to create material descriptor set layout");
    }
    pipeline->mMaterialDescriptorSetLayouts = std::move(materialDescriptorSetLayout);

    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo;
    auto descriptorSetLayouts = std::vector<vk::DescriptorSetLayout>{mGlobalDescriptorSetLayout.get(),
                                                                     pipeline->GetMaterialDescriptorSetLayout()};
    vk::PushConstantRange pushConstantRange;
    pushConstantRange.setSize(sizeof(glm::mat4))
        .setOffset(0)
        .setStageFlags(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment);
    pipelineLayoutCreateInfo.setSetLayouts(descriptorSetLayouts).setPushConstantRanges(pushConstantRange);
    auto pipelineLayout = mVulkanContext->GetDevice().createPipelineLayoutUnique(pipelineLayoutCreateInfo);
    if (!pipelineLayout)
    {
        LogError("Failed to create pipeline layout");
        throw std::runtime_error("Failed to create pipeline layout");
    }
    LogDebug("Pipeline layout created successfully");
    pipeline->mPipelineLayout = std::move(pipelineLayout);
    // 创建pipeline
    // ========== 1. 顶点输入状态 ==========
    auto vertexBindingDescription = Vertex::GetVertexInputBindingDescription();
    auto vertexInputAttributeDescriptions = Vertex::GetVertexInputAttributeDescription();
    auto vertexAttributeDescriptions = std::vector<vk::VertexInputAttributeDescription>(
        vertexInputAttributeDescriptions.begin(), vertexInputAttributeDescriptions.end());
    vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.setVertexBindingDescriptions(vertexBindingDescription)
        .setVertexAttributeDescriptions(vertexAttributeDescriptions);
    // ========== 2. 输入装配状态 ==========
    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
    inputAssemblyInfo.setTopology(vk::PrimitiveTopology::eTriangleList).setPrimitiveRestartEnable(vk::False);
    // ========== 3. 着色器阶段 ==========
    std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStages = {vk::PipelineShaderStageCreateInfo()
                                                                         .setStage(vk::ShaderStageFlagBits::eVertex)
                                                                         .setModule(pipeline->GetVertexShaderModule())
                                                                         .setPName("main"),
                                                                     vk::PipelineShaderStageCreateInfo()
                                                                         .setStage(vk::ShaderStageFlagBits::eFragment)
                                                                         .setModule(pipeline->GetFragmentShaderModule())
                                                                         .setPName("main")};
    // ========== 4. 视口和裁剪 ==========
    // Swapchain的宽高和Surface的宽高一致
    vk::Viewport viewport{};
    vk::Rect2D scissor{};
    vk::PipelineViewportStateCreateInfo viewportInfo;
    viewportInfo.setViewports(viewport).setScissors(scissor);
    // ========== 5. 光栅化状态 ==========
    vk::PipelineRasterizationStateCreateInfo rasterizationInfo{};
    rasterizationInfo.setDepthClampEnable(pipeline->mSetting.DepthClampEnable)
        .setRasterizerDiscardEnable(pipeline->mSetting.RasterizerDiscardEnable)
        .setPolygonMode(pipeline->mSetting.PolygonMode)
        .setLineWidth(pipeline->mSetting.LineWidth)
        .setCullMode(pipeline->mSetting.CullMode)
        .setFrontFace(pipeline->mSetting.FrontFace)
        .setDepthBiasEnable(pipeline->mSetting.DepthBiasEnable);
    // ========= 6. 多重采样 ==========
    vk::PipelineMultisampleStateCreateInfo multisampleInfo{};
    multisampleInfo.setSampleShadingEnable(pipeline->mSetting.MultisamplingEnable)
        .setRasterizationSamples(pipeline->mSetting.SampleCount)
        .setMinSampleShading(pipeline->mSetting.MinSampleShading)
        .setPSampleMask(nullptr)
        .setAlphaToCoverageEnable(pipeline->mSetting.AlphaToCoverageEnable)
        .setAlphaToOneEnable(pipeline->mSetting.AlphaToOneEnable);
    // ========== 7. 深度模板测试 ==========
    vk::PipelineDepthStencilStateCreateInfo depthStencilInfo{};
    depthStencilInfo.setDepthTestEnable(pipeline->mSetting.DepthTestEnable)
        .setDepthWriteEnable(pipeline->mSetting.DepthWriteEnable)
        .setDepthCompareOp(pipeline->mSetting.DepthCompareOp)
        .setDepthBoundsTestEnable(pipeline->mSetting.DepthBoundsTestEnable)
        .setMinDepthBounds(pipeline->mSetting.MinDepthBounds)
        .setMaxDepthBounds(pipeline->mSetting.MaxDepthBounds)
        .setStencilTestEnable(pipeline->mSetting.StencilTestEnable);
    // ========== 8. 颜色混合状态 ==========
    vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.setBlendEnable(pipeline->mSetting.ColorBlendingEnable)
        .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                           vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
    vk::PipelineColorBlendStateCreateInfo colorBlendInfo{};
    colorBlendInfo.setLogicOpEnable(pipeline->mSetting.LogicOpEnable)
        .setLogicOp(pipeline->mSetting.LogicOp)
        .setAttachments(colorBlendAttachment)
        .setBlendConstants({0.0f, 0.0f, 0.0f, 0.0f});
    // ========== 9. 动态状态 ==========
    std::vector<vk::DynamicState> dynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    vk::PipelineDynamicStateCreateInfo dynamicStateInfo{};
    dynamicStateInfo.setDynamicStates(dynamicStates);
    // ========== 10. 管线布局 ==========
    // ========== 11. 渲染通道 ==========
    auto renderPass = mRenderPassManager->GetRenderPass(pipeline->mSetting.RenderPassType);
    // ========== 12. 管线创建 ==========
    vk::GraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.setStageCount(static_cast<uint32_t>(shaderStages.size()))
        .setPStages(shaderStages.data())
        .setPVertexInputState(&vertexInputInfo)
        .setPInputAssemblyState(&inputAssemblyInfo)
        .setPViewportState(&viewportInfo)
        .setPRasterizationState(&rasterizationInfo)
        .setPMultisampleState(&multisampleInfo)
        .setPDepthStencilState(&depthStencilInfo)
        .setPColorBlendState(&colorBlendInfo)
        .setPDynamicState(&dynamicStateInfo)
        .setLayout(pipeline->mPipelineLayout.get())
        .setRenderPass(renderPass)
        .setSubpass(0);
    auto pipelineResult =
        mVulkanContext->GetDevice().createGraphicsPipelineUnique(vk::PipelineCache(), pipelineInfo, nullptr);
    if (pipelineResult.result != vk::Result::eSuccess)
    {
        LogError("Failed to create Forward Forward Opaque PBR pipeline");
    }
    pipeline->mPipeline = std::move(pipelineResult.value);
}
void MPipelineManager::Update(std::shared_ptr<MPipeline> pipeline)
{
    mAssets[pipeline->GetID()] = pipeline;
    mPipelines[pipeline->GetName()] = pipeline;
}
vk::UniqueShaderModule MPipelineManager::CreateShaderModule(const std::filesystem::path &shaderPath) const

{
    if (!std::filesystem::exists(shaderPath))
    {
        LogError("Shader file does not exist: {}", shaderPath.string());
        throw std::runtime_error("Shader file does not exist: " + shaderPath.string());
    }
    auto extension = shaderPath.extension();
    std::ifstream shaderFile(shaderPath, std::ios::in);
    if (!shaderFile.is_open())
    {
        LogError("Failed to open shader file: {}", shaderPath.string());
        throw std::runtime_error("Failed to open shader file: " + shaderPath.string());
    }
    // compiler
    std::stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();
    std::string shaderCode = shaderStream.str();
    auto kind = Utils::ShaderUtils::GetShaderKindFromExtension(extension.string());
    auto result = Utils::ShaderUtils::CompileShader(shaderCode, kind, shaderPath.string());
    std::vector<uint32_t> spirv(result.cbegin(), result.cend());
    vk::ShaderModuleCreateInfo shaderModuleCreateInfo;
    shaderModuleCreateInfo.setCode(spirv);
    auto shaderModule = mVulkanContext->GetDevice().createShaderModuleUnique(shaderModuleCreateInfo);
    if (!shaderModule)
    {
        LogError("Failed to create shader module from file: {}", shaderPath.string());
        throw std::runtime_error("Failed to create shader module from file: " + shaderPath.string());
    }

    return shaderModule;
}
void MPipelineManager::RemoveByName(const std::string &name)
{
    auto pipeline = GetByName(name);
    if (!pipeline)
    {
        LogError("Failed to remove pipeline: Pipeline with name {} does not exist", name);
        return;
    }
    Remove(pipeline->GetID());
}
std::shared_ptr<MPipeline> MPipelineManager::GetByName(const std::string &name) const
{
    if (mPipelines.contains(name))
    {
        return mPipelines.at(name);
    }
    return nullptr;
}
void MPipelineManager::Remove(const UUID &id)
{
    auto pipeline = Get(id);
    if (!pipeline)
    {
        LogError("Failed to remove pipeline: Pipeline with ID {} does not exist", id.ToString());
        return;
    }
    if (mPipelines.contains(pipeline->GetName()))
    {
        mPipelines.erase(pipeline->GetName());
    }
    Remove(id);
}
void MPipelineManager::CreateDefault()
{
    // ForwardOpaquePBR
    std::vector<vk::DescriptorSetLayoutBinding> mDescriptorSetLayoutBindings{

        vk::DescriptorSetLayoutBinding{0, vk::DescriptorType::eUniformBuffer, 1,
                                       vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment},
        // Binding: 1 Albedo
        vk::DescriptorSetLayoutBinding{1, vk::DescriptorType::eCombinedImageSampler, 1,
                                       vk::ShaderStageFlagBits::eFragment},
        // Binding: 2 Normal Map
        vk::DescriptorSetLayoutBinding{2, vk::DescriptorType::eCombinedImageSampler, 1,
                                       vk::ShaderStageFlagBits::eFragment},
        // Binding: 3 ARM (Ambient Occlusion, Roughness, Metallic)
        vk::DescriptorSetLayoutBinding{3, vk::DescriptorType::eCombinedImageSampler, 1,
                                       vk::ShaderStageFlagBits::eFragment},
        // Binding: 4 Emissive
        vk::DescriptorSetLayoutBinding{4, vk::DescriptorType::eCombinedImageSampler, 1,
                                       vk::ShaderStageFlagBits::eFragment}};
    auto pbrSetting = MPipelineSetting{};
    pbrSetting.VertexShaderPath = "Assets/Shaders/ForwardOpaquePBR.vert";
    pbrSetting.FragmentShaderPath = "Assets/Shaders/ForwardOpaquePBR.frag";
    pbrSetting.RenderPassType = RenderPassType::ForwardComposition;
    pbrSetting.MaterialDescriptorSetLayoutBindings = mDescriptorSetLayoutBindings;
    auto pbrPipeline = Create(PipelineType::ForwardOpaquePBR, pbrSetting);
    CreateVulkanResources(pbrPipeline);
}

} // namespace MEngine::Core::Manager
