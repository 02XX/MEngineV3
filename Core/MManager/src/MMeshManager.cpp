#include "MMeshManager.hpp"
#include "Logger.hpp"
#include "VMA.hpp"
#include <cstring>

namespace MEngine::Core::Manager
{
MMeshManager::MMeshManager(std::shared_ptr<VulkanContext> vulkanContext, std::shared_ptr<IUUIDGenerator> uuidGenerator)
    : MManager(vulkanContext, uuidGenerator)
{
    // 创建命令缓冲区
    auto device = vulkanContext->GetDevice();
    vk::CommandBufferAllocateInfo commandBufferAllocateInfo{};
    commandBufferAllocateInfo.setCommandPool(vulkanContext->GetTransferCommandPool())
        .setLevel(vk::CommandBufferLevel::ePrimary)
        .setCommandBufferCount(1);
    mCommandBuffer = std::move(device.allocateCommandBuffersUnique(commandBufferAllocateInfo)[0]);
    if (!mCommandBuffer)
    {
        LogError("Failed to create command buffer for MTextureManager");
        throw std::runtime_error("Failed to create command buffer for MTextureManager");
    }
    // 创建同步对象
    vk::FenceCreateInfo fenceCreateInfo{};
    mFence = device.createFenceUnique(fenceCreateInfo);
    if (!mFence)
    {
        LogError("Failed to create fence for MTextureManager");
        throw std::runtime_error("Failed to create fence for MTextureManager");
    }
    CreateDefault();
}
std::shared_ptr<MMesh> MMeshManager::Create(const MMeshSetting &setting, const std::string &name)
{
    std::shared_ptr<MMesh> mesh = std::make_shared<MMesh>(mUUIDGenerator->Create(), name, mVulkanContext, setting);
    vk::BufferCreateInfo vertexBufferCreateInfo{};
    vertexBufferCreateInfo.setSize(setting.vertexBufferSize)
        .setUsage(vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst)
        .setSharingMode(vk::SharingMode::eExclusive);
    VmaAllocationCreateInfo vertexBufferAllocationCreateInfo{};
    vertexBufferAllocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    if (vmaCreateBuffer(mVulkanContext->GetVmaAllocator(), &static_cast<VkBufferCreateInfo &>(vertexBufferCreateInfo),
                        &vertexBufferAllocationCreateInfo, reinterpret_cast<VkBuffer *>(&mesh->mVertexBuffer),
                        &mesh->mVertexBufferAllocation, &mesh->mVertexBufferAllocationInfo) != VK_SUCCESS)
    {
        LogError("Failed to create vertex buffer for mesh");
        throw std::runtime_error("Failed to create vertex buffer for mesh");
    }
    vk::BufferCreateInfo indexBufferCreateInfo{};
    indexBufferCreateInfo.setSize(setting.indexBufferSize)
        .setUsage(vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst)
        .setSharingMode(vk::SharingMode::eExclusive);
    VmaAllocationCreateInfo indexBufferAllocationCreateInfo{};
    indexBufferAllocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    if (vmaCreateBuffer(mVulkanContext->GetVmaAllocator(), &static_cast<VkBufferCreateInfo &>(indexBufferCreateInfo),
                        &indexBufferAllocationCreateInfo, reinterpret_cast<VkBuffer *>(&mesh->mIndexBuffer),
                        &mesh->mIndexBufferAllocation, &mesh->mIndexBufferAllocationInfo) != VK_SUCCESS)
    {
        LogError("Failed to create index buffer for mesh");
        throw std::runtime_error("Failed to create index buffer for mesh");
    }
    mAssets[mesh->GetID()] = mesh;
    return mesh;
}
void MMeshManager::WriteBuffer(vk::Buffer buffer, void *data, uint32_t size)
{
    mCommandBuffer->reset();
    mVulkanContext->GetDevice().resetFences(mFence.get());
    vk::Buffer stagingBuffer;
    vk::BufferCreateInfo stagingBufferCreateInfo{};
    stagingBufferCreateInfo.setSize(size)
        .setUsage(vk::BufferUsageFlagBits::eTransferSrc)
        .setSharingMode(vk::SharingMode::eExclusive);
    VmaAllocationCreateInfo stagingAllocationCreateInfo{};
    stagingAllocationCreateInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
    stagingAllocationCreateInfo.flags =
        VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
    VmaAllocation stagingAllocation;
    VmaAllocationInfo stagingAllocationInfo;
    if (vmaCreateBuffer(mVulkanContext->GetVmaAllocator(), &static_cast<VkBufferCreateInfo &>(stagingBufferCreateInfo),
                        &stagingAllocationCreateInfo, reinterpret_cast<VkBuffer *>(&stagingBuffer), &stagingAllocation,
                        &stagingAllocationInfo) != VK_SUCCESS)
    {
        LogError("Failed to create staging buffer");
    }
    memcpy(stagingAllocationInfo.pMappedData, data, size);
    mCommandBuffer->begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
    {
        vk::BufferCopy copyRegion{};
        copyRegion.setSize(size).setDstOffset(0).setSrcOffset(0);
        mCommandBuffer->copyBuffer(stagingBuffer, buffer, 1, &copyRegion);
    }
    mCommandBuffer->end();
    vk::SubmitInfo submitInfo{};
    submitInfo.setCommandBuffers(mCommandBuffer.get());
    mVulkanContext->GetTransferQueue().submit(submitInfo, mFence.get());
    // 等待提交完成
    auto result = mVulkanContext->GetDevice().waitForFences(mFence.get(), vk::True, 100000000); // 10秒超时
    if (result != vk::Result::eSuccess)
    {
        LogError("Failed to wait for fence: {}", vk::to_string(result));
        throw std::runtime_error("Failed to wait for fence");
    }
    // 清理临时资源
    vmaDestroyBuffer(mVulkanContext->GetVmaAllocator(), stagingBuffer, stagingAllocation);
}
void MMeshManager::Write(std::shared_ptr<MMesh> mesh, const std::vector<Vertex> &vertices,
                         const std::vector<uint32_t> &indices)
{
    // vertex Staging buffer
    WriteBuffer(mesh->GetVertexBuffer(), const_cast<Vertex *>(vertices.data()),
                static_cast<uint32_t>(vertices.size() * sizeof(Vertex)));
    // index Staging buffer
    WriteBuffer(mesh->GetIndexBuffer(), const_cast<uint32_t *>(indices.data()),
                static_cast<uint32_t>(indices.size() * sizeof(uint32_t)));
}
void MMeshManager::CreateDefault()
{
}
}; // namespace MEngine::Core::Manager
