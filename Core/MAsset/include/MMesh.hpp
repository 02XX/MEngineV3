#pragma once
#include "MAsset.hpp"
#include "MManager_fwd.hpp"
#include "VMA.hpp"
#include "Vertex.hpp"
#include <nlohmann/json_fwd.hpp>
#include <vector>
#include <vulkan/vulkan_handles.hpp>

namespace MEngine::Core::Asset
{
class MMeshSetting : public MAssetSetting
{
  public:
    uint32_t vertexBufferSize = 0;
    uint32_t indexBufferSize = 0;
    vk::BufferUsageFlags vertexBufferUsage =
        vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst;
    vk::BufferUsageFlags indexBufferUsage =
        vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst;
};

class MMesh : public MAsset
{
    friend class nlohmann::adl_serializer<MMesh>;
    friend class Manager::MMeshManager;

  private:
    std::shared_ptr<VulkanContext> mVulkanContext;
    std::vector<Vertex> mVertices;
    std::vector<uint32_t> mIndices;

    vk::Buffer mVertexBuffer;
    vk::Buffer mIndexBuffer;
    VmaAllocation mVertexBufferAllocation;
    VmaAllocation mIndexBufferAllocation;
    VmaAllocationInfo mVertexBufferAllocationInfo;
    VmaAllocationInfo mIndexBufferAllocationInfo;

    MMeshSetting mSetting;

  public:
    MMesh(const UUID &id, std::shared_ptr<VulkanContext> vulkanContext, const MMeshSetting &setting)
        : MAsset(id), mSetting(setting), mVulkanContext(vulkanContext)
    {
        mType = MAssetType::Mesh;
        mState = MAssetState::Unloaded;
    }
    ~MMesh() override
    {
        if (mVertexBufferAllocation)
        {
            vmaDestroyBuffer(mVulkanContext->GetVmaAllocator(), mVertexBuffer, mVertexBufferAllocation);
        }
        if (mIndexBufferAllocation)
        {
            vmaDestroyBuffer(mVulkanContext->GetVmaAllocator(), mIndexBuffer, mIndexBufferAllocation);
        }
    }
    inline const std::vector<Vertex> &GetVertices() const
    {
        return mVertices;
    }
    inline const std::vector<uint32_t> &GetIndices() const
    {
        return mIndices;
    }
    inline void SetVertices(const std::vector<Vertex> &vertices)
    {
        mVertices = vertices;
    }
    inline void SetIndices(const std::vector<uint32_t> &indices)
    {
        mIndices = indices;
    }
    inline const vk::Buffer GetVertexBuffer() const
    {
        return mVertexBuffer;
    }
    inline const vk::Buffer GetIndexBuffer() const
    {
        return mIndexBuffer;
    }
    inline const MMeshSetting &GetSetting() const
    {
        return mSetting;
    }
    inline void SetSetting(const MMeshSetting &setting)
    {
        mSetting = setting;
    }
};
} // namespace MEngine::Core::Asset
