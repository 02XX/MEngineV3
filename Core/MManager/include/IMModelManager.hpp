#pragma once
#include "IMManager.hpp"
#include "MModel.hpp"
#include <vulkan/vulkan_handles.hpp>
using namespace MEngine::Core::Asset;
namespace MEngine::Core::Manager
{
class IMModelManager : public virtual IMManager<MModel>
{
  public:
    ~IMModelManager() override = default;
    virtual std::shared_ptr<MModel> Create(const std::string &name, const std::vector<UUID> &meshIDs,
                                           const std::vector<UUID> &materialIDs, std::unique_ptr<Node> rootNode,
                                           const MModelSetting &setting) = 0;
    virtual std::shared_ptr<MModel> CreateCube() = 0;
    virtual std::shared_ptr<MModel> CreateSphere() = 0;
    virtual std::shared_ptr<MModel> CreatePlane() = 0;
    virtual std::shared_ptr<MModel> CreateCylinder() = 0;
};
} // namespace MEngine::Core::Manager