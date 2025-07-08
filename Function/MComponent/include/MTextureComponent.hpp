#pragma once
#include "MComponent.hpp"
#include "MTexture.hpp"
#include <imgui.h>
using namespace MEngine::Core;
namespace MEngine::Function::Component
{

struct MTextureComponent : public MComponent
{
    UUID textureID;
    std::shared_ptr<MEngine::Core::Asset::MTexture> texture;
    ImTextureID imguiTextureID;
};

} // namespace MEngine::Function::Component