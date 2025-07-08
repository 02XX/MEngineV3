#pragma once
#include "IMManager.hpp"
#include "MMesh.hpp"
#include <vector>
#include <vulkan/vulkan_handles.hpp>
using namespace MEngine::Core::Asset;
namespace MEngine::Core::Manager
{
class IMMeshManager : public virtual IMManager<MMesh, MMeshSetting>
{
  public:
    ~IMMeshManager() override = default;
    virtual void Write(std::shared_ptr<MMesh> mesh, const std::vector<Vertex> &vertices,
                       const std::vector<uint32_t> &indices) = 0;
};
} // namespace MEngine::Core::Manager