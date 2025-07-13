#pragma once
#include "MAsset.hpp"
#include "MManager_fwd.hpp"
#include "VMA.hpp"
#include "Vertex.hpp"
#include "VulkanContext.hpp"
#include <cstdint>
#include <nlohmann/json_fwd.hpp>
#include <vector>
#include <vulkan/vulkan_handles.hpp>


namespace MEngine::Core::Asset
{
class MMeshSetting : public MAssetSetting
{
  public:
    ~MMeshSetting() override = default;
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
    MMesh(const UUID &id, const std::string &name, std::shared_ptr<VulkanContext> vulkanContext,
          const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices, const MMeshSetting &setting)
        : MAsset(id, name), mSetting(setting), mVulkanContext(vulkanContext), mVertices(vertices), mIndices(indices)
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
    inline uint32_t GetIndexCount() const
    {
        return static_cast<uint32_t>(mIndices.size());
    }
};
} // namespace MEngine::Core::Asset
