#pragma once
#include "MComponent.hpp"
#include "MMaterial.hpp"
#include "UUID.hpp"
#include <memory>

using namespace MEngine::Core;
namespace MEngine::Function::Component
{

struct MMaterialComponent : public MComponent
{
    UUID materialID;
    std::shared_ptr<Asset::MMaterial> material;
};

} // namespace MEngine::Function::Component
