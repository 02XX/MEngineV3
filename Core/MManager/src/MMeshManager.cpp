#include "MMeshManager.hpp"
#include "IMMeshManager.hpp"
#include "Logger.hpp"
#include "VMA.hpp"
#include <cstring>
#include <glm/ext/scalar_constants.hpp>

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
std::shared_ptr<MMesh> MMeshManager::Create(const std::string &name, const std::vector<Vertex> &vertices,
                                            const std::vector<uint32_t> &indices, const MMeshSetting &setting)
{
    std::shared_ptr<MMesh> mesh =
        std::make_shared<MMesh>(mUUIDGenerator->Create(), name, mVulkanContext, vertices, indices, setting);
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
void MMeshManager::Update(std::shared_ptr<MMesh> mesh)
{
    mAssets[mesh->GetID()] = mesh;
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
void MMeshManager::Write(std::shared_ptr<MMesh> mesh)
{
    // vertex Staging buffer
    WriteBuffer(mesh->GetVertexBuffer(), const_cast<Vertex *>(mesh->mVertices.data()),
                static_cast<uint32_t>(mesh->mVertices.size() * sizeof(Vertex)));
    // index Staging buffer
    WriteBuffer(mesh->GetIndexBuffer(), const_cast<uint32_t *>(mesh->mIndices.data()),
                static_cast<uint32_t>(mesh->mIndices.size() * sizeof(uint32_t)));
}
void MMeshManager::CreateDefault()
{
    auto cubeMesh = CreateCubeMesh();
    auto sphereMesh = CreateSphereMesh();
    auto planeMesh = CreatePlaneMesh();
    auto cylinderMesh = CreateCylinderMesh();
    Remove(cubeMesh->mID);
    Remove(sphereMesh->mID);
    Remove(planeMesh->mID);
    Remove(cylinderMesh->mID);
    cubeMesh->mID = mDefaultMeshes[DefaultMeshType::Cube];
    sphereMesh->mID = mDefaultMeshes[DefaultMeshType::Sphere];
    planeMesh->mID = mDefaultMeshes[DefaultMeshType::Plane];
    cylinderMesh->mID = mDefaultMeshes[DefaultMeshType::Cylinder];

    mAssets[mDefaultMeshes[DefaultMeshType::Cube]] = cubeMesh;
    mAssets[mDefaultMeshes[DefaultMeshType::Sphere]] = sphereMesh;
    mAssets[mDefaultMeshes[DefaultMeshType::Plane]] = planeMesh;
    mAssets[mDefaultMeshes[DefaultMeshType::Cylinder]] = cylinderMesh;
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
    auto mesh = Create("Cube Mesh", vertices, indices, meshSetting);
    CreateVulkanResources(mesh);
    Write(mesh);
    return mesh;
}
std::shared_ptr<MMesh> MMeshManager::CreateSphereMesh()
{
    const unsigned int sectorCount = 36; // 纬度
    const unsigned int stackCount = 18;  // 经度

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    float radius = 0.5f; // 球体半径

    // 顶点计算
    for (unsigned int i = 0; i <= stackCount; ++i)
    {
        float stackAngle = glm::pi<float>() / 2 - i * glm::pi<float>() / stackCount; // 计算每个纬度
        float xy = radius * cosf(stackAngle);                                        // 当前纬度半径
        float z = radius * sinf(stackAngle);                                         // Z轴坐标

        // 计算每个经度的顶点
        for (unsigned int j = 0; j <= sectorCount; ++j)
        {
            float sectorAngle = 2 * glm::pi<float>() * j / sectorCount; // 每个经度的角度

            // 计算顶点的坐标
            float x = xy * cosf(sectorAngle); // X坐标
            float y = xy * sinf(sectorAngle); // Y坐标
            vertices.push_back({
                {x, y, z},                                      // 位置
                {x / radius, y / radius, z / radius},           // 法线
                {float(j) / sectorCount, float(i) / stackCount} // UV坐标
            });
        }
    }

    // 索引计算
    for (unsigned int i = 0; i < stackCount; ++i)
    {
        unsigned int k1 = i * (sectorCount + 1); // 当前纬度的起始顶点
        unsigned int k2 = k1 + sectorCount + 1;  // 下一纬度的起始顶点

        for (unsigned int j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            if (i != 0)
            {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            if (i != (stackCount - 1))
            {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    auto meshSetting = MMeshSetting{};
    auto mesh = Create("Sphere Mesh", vertices, indices, meshSetting);
    CreateVulkanResources(mesh);
    Write(mesh);
    return mesh;
}
std::shared_ptr<MMesh> MMeshManager::CreatePlaneMesh()
{
    // 平面顶点
    const std::vector<Vertex> vertices = {
        // 顶面（Z+）
        {{-0.5f, 0.0f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},  // 0
        {{0.5f, 0.0f, 0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},   // 1
        {{0.5f, 0.0f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},  // 2
        {{-0.5f, 0.0f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}}, // 3
    };

    // 索引数据
    const std::vector<uint32_t> indices = {0, 1, 2, 0, 2, 3};

    auto meshSetting = MMeshSetting{};
    auto mesh = Create("Plane Mesh", vertices, indices, meshSetting);
    CreateVulkanResources(mesh);
    Write(mesh);
    return mesh;
}
std::shared_ptr<MMesh> MMeshManager::CreateCylinderMesh()
{
    const unsigned int sectorCount = 36; // 圆周分段数
    const float radius = 0.5f;           // 圆柱半径
    const float halfHeight = 0.5f;       // 半高（圆柱总高度为1.0）

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    // ====================
    // 1. 生成圆柱侧面
    // ====================
    for (unsigned int i = 0; i <= 1; ++i)
    { // 上下两个圆环
        float y = i ? -halfHeight : halfHeight;
        for (unsigned int j = 0; j <= sectorCount; ++j)
        {
            float sectorAngle = 2 * glm::pi<float>() * j / sectorCount;
            float x = radius * cosf(sectorAngle);
            float z = radius * sinf(sectorAngle);

            // 法线在xz平面向外（Y轴为0）
            glm::vec3 normal(cosf(sectorAngle), 0.0f, sinf(sectorAngle));

            // UV坐标：u沿圆周，v沿高度
            float u = static_cast<float>(j) / sectorCount;
            float v = i ? 0.0f : 1.0f; // 顶部v=1.0，底部v=0.0

            vertices.push_back({
                {x, y, z}, // 位置
                normal,    // 法线
                {u, v}     // UV坐标
            });
        }
    }

    // 侧面索引（四边形带）
    for (unsigned int i = 0; i < sectorCount; ++i)
    {
        unsigned int topIndex = i;
        unsigned int bottomIndex = (sectorCount + 1) + i;

        // 第一个三角形
        indices.push_back(topIndex);
        indices.push_back(bottomIndex);
        indices.push_back(topIndex + 1);

        // 第二个三角形
        indices.push_back(topIndex + 1);
        indices.push_back(bottomIndex);
        indices.push_back(bottomIndex + 1);
    }

    // ====================
    // 2. 生成顶部圆盘
    // ====================
    unsigned int topCenterIndex = vertices.size();
    vertices.push_back({
        {0.0f, halfHeight, 0.0f}, // 中心点
        {0.0f, 1.0f, 0.0f},       // 法线向上
        {0.5f, 0.5f}              // UV中心
    });

    for (unsigned int i = 0; i <= sectorCount; ++i)
    {
        float sectorAngle = 2 * glm::pi<float>() * i / sectorCount;
        float x = radius * cosf(sectorAngle);
        float z = radius * sinf(sectorAngle);

        // UV映射到圆盘 [0,1]
        float u = 0.5f + 0.5f * cosf(sectorAngle);
        float v = 0.5f + 0.5f * sinf(sectorAngle);

        vertices.push_back({
            {x, halfHeight, z}, // 在顶部平面
            {0.0f, 1.0f, 0.0f}, // 法线向上
            {u, v}              // UV坐标
        });
    }

    // 顶部索引
    for (unsigned int i = 0; i < sectorCount; ++i)
    {
        indices.push_back(topCenterIndex);
        indices.push_back(topCenterIndex + 1 + i);
        indices.push_back(topCenterIndex + 1 + i + 1);
    }

    // ====================
    // 3. 生成底部圆盘
    // ====================
    unsigned int bottomCenterIndex = vertices.size();
    vertices.push_back({
        {0.0f, -halfHeight, 0.0f}, // 中心点
        {0.0f, -1.0f, 0.0f},       // 法线向下
        {0.5f, 0.5f}               // UV中心
    });

    for (unsigned int i = 0; i <= sectorCount; ++i)
    {
        float sectorAngle = 2 * glm::pi<float>() * i / sectorCount;
        float x = radius * cosf(sectorAngle);
        float z = radius * sinf(sectorAngle);

        // UV映射到圆盘 [0,1] (与顶部相同)
        float u = 0.5f + 0.5f * cosf(sectorAngle);
        float v = 0.5f + 0.5f * sinf(sectorAngle);

        vertices.push_back({
            {x, -halfHeight, z}, // 在底部平面
            {0.0f, -1.0f, 0.0f}, // 法线向下
            {u, v}               // UV坐标
        });
    }

    // 底部索引（注意缠绕顺序翻转）
    for (unsigned int i = 0; i < sectorCount; ++i)
    {
        indices.push_back(bottomCenterIndex);
        indices.push_back(bottomCenterIndex + 1 + i + 1);
        indices.push_back(bottomCenterIndex + 1 + i);
    }

    // ====================
    // 4. 创建网格
    // ====================
    auto meshSetting = MMeshSetting{};
    auto mesh = Create("Cylinder Mesh", vertices, indices, meshSetting);
    CreateVulkanResources(mesh);
    Write(mesh);
    return mesh;
}
std::shared_ptr<MMesh> MMeshManager::GetMesh(DefaultMeshType type) const
{
    if (mDefaultMeshes.find(type) != mDefaultMeshes.end())
    {
        return Get(mDefaultMeshes.at(type));
    }
    LogError("Default mesh type {} not found", static_cast<int>(type));
    return nullptr;
}
}; // namespace MEngine::Core::Manager
