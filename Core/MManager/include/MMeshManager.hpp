#pragma once
#include "IMMeshManager.hpp"
#include "MManager.hpp"
#include "RenderPassManager.hpp"
#include "UUID.hpp"
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vulkan/vulkan_handles.hpp>

using namespace MEngine::Core::Asset;
namespace MEngine::Core::Manager
{

class MMeshManager final : public MManager<MMesh>, public IMMeshManager
{
  private:
    std::shared_ptr<RenderPassManager> mRenderPassManager;
    vk::UniqueCommandBuffer mCommandBuffer;
    vk::UniqueFence mFence;
    void WriteBuffer(vk::Buffer buffer, void *data, uint32_t size);
    std::unordered_map<DefaultMeshType, UUID> mDefaultMeshes{
        {DefaultMeshType::Cube, UUID{"00000000-0000-0000-0000-000000000001"}},
        {DefaultMeshType::Cylinder, UUID{"00000000-0000-0000-0000-000000000002"}},
        {DefaultMeshType::Plane, UUID{"00000000-0000-0000-0000-000000000003"}},
        {DefaultMeshType::Sphere, UUID{"00000000-0000-0000-0000-000000000004"}},
        {DefaultMeshType::Sky, UUID{"00000000-0000-0000-0000-000000000005"}},
    };

  public:
    MMeshManager(std::shared_ptr<VulkanContext> vulkanContext, std::shared_ptr<IUUIDGenerator> uuidGenerator);
    ~MMeshManager() override = default;
    std::shared_ptr<MMesh> Create(const std::string &name, const std::vector<Vertex> &vertices,
                                  const std::vector<uint32_t> &indices, const MMeshSetting &setting) override;
    void Update(std::shared_ptr<MMesh> mesh) override;
    void Write(std::shared_ptr<MMesh> mesh) override;
    void CreateDefault() override;
    virtual void CreateVulkanResources(std::shared_ptr<MMesh> asset) override;
    std::shared_ptr<MMesh> CreateCubeMesh() override;
    std::shared_ptr<MMesh> CreateSphereMesh() override;
    std::shared_ptr<MMesh> CreatePlaneMesh() override;
    std::shared_ptr<MMesh> CreateCylinderMesh() override;
    std::shared_ptr<MMesh> CreateSkyMesh() override;
    std::shared_ptr<MMesh> GetMesh(DefaultMeshType type) const override;
};
} // namespace MEngine::Core::Manager