#pragma once
#include "IMModelManager.hpp"
#include "MModel.hpp"
#include "ResourceManager.hpp"
#include <entt/entt.hpp>
#include <memory>
using namespace MEngine::Core::Asset;
namespace MEngine::Function::Utils
{
class EntityUtils
{
  public:
    static entt::entity CreateEntity(std::shared_ptr<entt::registry> registry, std::shared_ptr<MModel> model);
    static std::shared_ptr<MModel> GetModelFromEntity(std::shared_ptr<Resource::ResourceManager> resourceManager,
                                                      std::shared_ptr<entt::registry> registry, entt::entity entity);
};
} // namespace MEngine::Function::Utils
