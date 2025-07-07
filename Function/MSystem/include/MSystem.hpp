#pragma once
#include "IMSystem.hpp"
#include "ResourceManager.hpp"
#include <entt/fwd.hpp>
#include <memory>

using namespace MEngine::Resource;

namespace MEngine::Function::System
{
class MSystem : public IMSystem
{
  protected:
    std::shared_ptr<entt::registry> mRegistry;
    std::shared_ptr<ResourceManager> mResourceManager;

  public:
    MSystem(std::shared_ptr<entt::registry> registry, std::shared_ptr<ResourceManager> resourceManager)
        : mRegistry(registry), mResourceManager(resourceManager)
    {
    }
    virtual ~MSystem() = default;
};

} // namespace MEngine::Function::System