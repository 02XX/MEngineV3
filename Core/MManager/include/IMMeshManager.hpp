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
    Cylinder,
    Sky,
};
class IMMeshManager : public virtual IMManager<MMesh>
{
  public:
    ~IMMeshManager() override = default;
    virtual std::shared_ptr<MMesh> Create(const std::string &name, const std::vector<Vertex> &vertices,
                                          const std::vector<uint32_t> &indices, const MMeshSetting &setting) = 0;
    virtual void Write(std::shared_ptr<MMesh> mesh) = 0;
    virtual std::shared_ptr<MMesh> CreateCubeMesh() = 0;
    virtual std::shared_ptr<MMesh> CreateSphereMesh() = 0;
    virtual std::shared_ptr<MMesh> CreatePlaneMesh() = 0;
    virtual std::shared_ptr<MMesh> CreateCylinderMesh() = 0;
    virtual std::shared_ptr<MMesh> CreateSkyMesh() = 0;
    virtual std::shared_ptr<MMesh> GetMesh(DefaultMeshType type) const = 0;
};
} // namespace MEngine::Core::Manager