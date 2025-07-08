#pragma once
#include "IMMeshManager.hpp"
#include "MManager.hpp"
#include "RenderPassManager.hpp"
#include <cstdint>
#include <memory>
#include <vulkan/vulkan_handles.hpp>

using namespace MEngine::Core::Asset;
namespace MEngine::Core::Manager
{

class MMeshManager final : public MManager<MMesh, MMeshSetting>, public IMMeshManager
{
  private:
    std::shared_ptr<RenderPassManager> mRenderPassManager;
    vk::UniqueCommandBuffer mCommandBuffer;
    vk::UniqueFence mFence;
    void WriteBuffer(vk::Buffer buffer, void *data, uint32_t size);

  public:
    MMeshManager(std::shared_ptr<VulkanContext> vulkanContext, std::shared_ptr<IUUIDGenerator> uuidGenerator);
    ~MMeshManager() override = default;
    std::shared_ptr<MMesh> Create(const MMeshSetting &setting, const std::string &name = "New Mesh") override;
    void Update(std::shared_ptr<MMesh> mesh) override
    {
    }
    void Write(std::shared_ptr<MMesh> mesh, const std::vector<Vertex> &vertices,
               const std::vector<uint32_t> &indices) override;
    void CreateDefault() override;
};
} // namespace MEngine::Core::Manager