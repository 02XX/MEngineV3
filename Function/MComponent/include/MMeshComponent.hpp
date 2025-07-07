#pragma once

#include "MComponent.hpp"
#include "MModel.hpp"

namespace MEngine::Function::Component
{

struct MMeshComponent : public MComponent
{
    UUID meshID;
    std::shared_ptr<MEngine::Core::Asset::MMesh> mesh;
};

} // namespace MEngine::Function::Component