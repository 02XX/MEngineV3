#pragma once
#include "MModel.hpp"
#include <entt/entt.hpp>
#include <memory>
using namespace MEngine::Core::Asset;
namespace MEngine::Function::Utils
{
class EntityUtils
{
  public:
    static entt::entity CreateEntity(std::shared_ptr<entt::registry> registry, std::shared_ptr<MModel> model);
};
} // namespace MEngine::Function::Utils
