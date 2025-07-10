#pragma once
#include "IMManager.hpp"
#include "MMesh.hpp"
#include <memory>
#include <vector>
#include <vulkan/vulkan_handles.hpp>
using namespace MEngine::Core::Asset;
namespace MEngine::Core::Manager
{
enum class DefaultMeshType
{
    Cube,
    Sphere,
    Plane,
    Cylinder
};
class IMMeshManager : public virtual IMManager<MMesh, MMeshSetting>
{
  public:
    ~IMMeshManager() override = default;
    virtual void Write(std::shared_ptr<MMesh> mesh, const std::vector<Vertex> &vertices,
                       const std::vector<uint32_t> &indices) = 0;
    virtual std::shared_ptr<MMesh> CreateCubeMesh() = 0;
    virtual std::shared_ptr<MMesh> CreateSphereMesh() = 0;
    virtual std::shared_ptr<MMesh> CreatePlaneMesh() = 0;
    virtual std::shared_ptr<MMesh> CreateCylinderMesh() = 0;

    virtual std::shared_ptr<MMesh> GetMesh(DefaultMeshType type) const = 0;
};
} // namespace MEngine::Core::Manager