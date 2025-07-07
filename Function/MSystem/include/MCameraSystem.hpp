#pragma once
#include "MSystem.hpp"
#include <entt/fwd.hpp>
#include <memory>

namespace MEngine::Function::System
{
class MCameraSystem final : public MSystem
{
  private:
  public:
    MCameraSystem(std::shared_ptr<entt::registry> registry, std::shared_ptr<ResourceManager> resourceManager)
        : MSystem(registry, resourceManager)
    {
    }
    void Init() override;
    void Update(float deltaTime) override;
    void Shutdown() override;

  private:
    void CalculateMatrix(entt::entity entity);
};
} // namespace MEngine::Function::System