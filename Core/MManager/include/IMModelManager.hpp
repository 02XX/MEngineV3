#pragma once
#include "IMManager.hpp"
#include "MModel.hpp"
#include <vulkan/vulkan_handles.hpp>
using namespace MEngine::Core::Asset;
namespace MEngine::Core::Manager
{
class IMModelManager : public virtual IMManager<MModel, MModelSetting>
{
  public:
    ~IMModelManager() override = default;
    virtual std::shared_ptr<MModel> CreateCube() = 0;
    virtual std::shared_ptr<MModel> CreateSphere() = 0;
    virtual std::shared_ptr<MModel> CreatePlane() = 0;
    virtual std::shared_ptr<MModel> CreateCylinder() = 0;
};
} // namespace MEngine::Core::Manager