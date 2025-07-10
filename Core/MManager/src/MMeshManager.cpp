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
    mAssets[mesh->GetID()] = mesh;
    return mesh;
}
void MMeshManager::CreateVulkanResources(std::shared_ptr<MMesh> mesh)
{
    if (mesh->mVertexBuffer)
    {
        vmaDestroyBuffer(mVulkanContext->GetVmaAllocator(), mesh->mVertexBuffer, mesh->mVertexBufferAllocation);
    }
    if (mesh->mIndexBuffer)
    {
        vmaDestroyBuffer(mVulkanContext->GetVmaAllocator(), mesh->mIndexBuffer, mesh->mIndexBufferAllocation);
    }
    vk::BufferCreateInfo vertexBufferCreateInfo{};
    vertexBufferCreateInfo.setSize(mesh->mVertices.size() * sizeof(Vertex))
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
    indexBufferCreateInfo.setSize(mesh->mIndices.size() * sizeof(uint32_t))
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
    mDefaultMeshes[DefaultMeshType::Cube] = CreateCubeMesh();
}
std::shared_ptr<MMesh> MMeshManager::CreateCubeMesh()
{
    // Cube vertices and indices
    // 立方体顶点数据（每个面4个顶点，共24个顶点）
    const std::vector<Vertex> vertices = {
        // 前面 (Z+)
        {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // 0
        {{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},  // 1
        {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},   // 2
        {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},  // 3

        // 后面 (Z-)
        {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},  // 4
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}}, // 5
        {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},  // 6
        {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},   // 7

        // 左面 (X-)
        {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}, // 8
        {{-0.5f, -0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},  // 9
        {{-0.5f, 0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},   // 10
        {{-0.5f, 0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},  // 11

        // 右面 (X+)
        {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},  // 12
        {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}, // 13
        {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},  // 14
        {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},   // 15

        // 顶面 (Y+)
        {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},  // 16
        {{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},   // 17
        {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},  // 18
        {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}}, // 19

        // 底面 (Y-)
        {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}}, // 20
        {{0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},  // 21
        {{0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},   // 22
        {{-0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},  // 23
    };

    // 索引数据（每个面2个三角形，共36个索引，全部为逆时针顺序）
    const std::vector<uint32_t> indices = {// 前面 (Z+) 逆时针：0→1→2→3
                                           // 前面
                                           0, 1, 2, 0, 2, 3,
                                           // 后面
                                           4, 5, 6, 4, 6, 7,
                                           // 左面
                                           8, 9, 10, 8, 10, 11,
                                           // 右面
                                           12, 13, 14, 12, 14, 15,
                                           // 顶面
                                           16, 17, 18, 16, 18, 19,
                                           // 底面
                                           20, 21, 22, 20, 22, 23};
    auto meshSetting = MMeshSetting{};
    meshSetting.vertexBufferSize = vertices.size() * sizeof(Vertex);
    meshSetting.indexBufferSize = indices.size() * sizeof(uint32_t);
    auto mesh = Create(meshSetting, "Cube Mesh");
    mesh->SetVertices(vertices);
    mesh->SetIndices(indices);
    CreateVulkanResources(mesh);
    Write(mesh, vertices, indices);
    return mesh;
}
std::shared_ptr<MMesh> MMeshManager::CreateSphereMesh()
{
}
std::shared_ptr<MMesh> MMeshManager::CreatePlaneMesh()
{
}
std::shared_ptr<MMesh> MMeshManager::CreateCylinderMesh()
{
}
std::shared_ptr<MMesh> MMeshManager::GetMesh(DefaultMeshType type) const
{
    if (mDefaultMeshes.find(type) != mDefaultMeshes.end())
    {
        return mDefaultMeshes.at(type);
    }
    LogError("Default mesh type {} not found", static_cast<int>(type));
    return nullptr;
}
}; // namespace MEngine::Core::Manager
