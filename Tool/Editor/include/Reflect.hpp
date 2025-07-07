#include "MAsset.hpp"
#include "MCameraComponent.hpp"
#include "MMaterial.hpp"
#include "MMaterialComponent.hpp"
#include "MMeshComponent.hpp"
#include "MPipeline.hpp"
#include "MTexture.hpp"
#include "MTransformComponent.hpp"
#include <entt/meta/factory.hpp>

using namespace entt::literals;
using namespace MEngine::Core::Asset;
using namespace MEngine::Function::Component;
struct Info
{
    std::string DisplayName = "Unknown";
    bool Editable = false;
    bool Serializable = true;
};
static void RegisterAsset()
{
    entt::meta_factory<MAsset>()
        .type("Asset"_hs)
        .custom<Info>(Info{
            .DisplayName = "Asset",
            .Editable = false,
            .Serializable = true,
        })
        .data<&MAsset::SetID, &MAsset::GetID>("ID"_hs)
        .custom<Info>(Info{
            .DisplayName = "ID",
            .Editable = false,
            .Serializable = true,
        })
        .data<&MAsset::SetPath, &MAsset::GetPath>("Path"_hs)
        .custom<Info>(Info{
            .DisplayName = "Asset Path",
            .Editable = false,
            .Serializable = true,
        });
    entt::meta_factory<MTexture>()
        .base<MAsset>()
        .type("Texture"_hs)
        .custom<Info>(Info{
            .DisplayName = "Texture",
            .Editable = true,
            .Serializable = true,
        });
    entt::meta_factory<MPipeline>()
        .base<MAsset>()
        .type("Pipeline"_hs)
        .custom<Info>(Info{
            .DisplayName = "Pipeline",
            .Editable = true,
            .Serializable = true,
        });
}
static void RegisterSetting()
{
}
static void RegisterComponent()
{
    entt::meta_factory<MComponent>()
        .type("Component"_hs)
        .custom<Info>(Info{
            .DisplayName = "Component",
            .Serializable = false,
        })
        .data<&MComponent::dirty>("dirty"_hs)
        .custom<Info>(Info{
            .DisplayName = "dirty",
            .Serializable = true,
        });
    entt::meta_factory<MTransformComponent>()
        .type("TransformComponent"_hs)
        .custom<Info>(Info{
            .DisplayName = "TransformComponent",
        })
        .base<MComponent>()
        .data<&MTransformComponent::name>("name"_hs)
        .custom<Info>(Info{
            .DisplayName = "name",
            .Editable = true,
        })
        .data<&MTransformComponent::localPosition>("localPosition"_hs)
        .custom<Info>(Info{
            .DisplayName = "localPosition",
            .Editable = true,
        })
        .data<&MTransformComponent::localRotation>("localRotation"_hs)
        .custom<Info>(Info{
            .DisplayName = "localRotation",
            .Editable = true,
        })
        .data<&MTransformComponent::localScale>("localScale"_hs)
        .custom<Info>(Info{
            .DisplayName = "localScale",
            .Editable = true,
        })
        .data<&MTransformComponent::worldPosition>("worldPosition"_hs)
        .custom<Info>(Info{
            .DisplayName = "worldPosition",
            .Editable = false,
        })
        .data<&MTransformComponent::worldRotation>("worldRotation"_hs)
        .custom<Info>(Info{
            .DisplayName = "worldRotation",
            .Editable = false,
        })
        .data<&MTransformComponent::worldScale>("worldScale"_hs)
        .custom<Info>(Info{
            .DisplayName = "worldScale",
            .Editable = false,
        });
    entt::meta_factory<MCameraComponent>()
        .type("CameraComponent"_hs)
        .custom<Info>(Info{
            .DisplayName = "CameraComponent",
        })
        .base<MComponent>()
        .data<&MCameraComponent::isMainCamera>("isMainCamera"_hs)
        .custom<Info>(Info{
            .DisplayName = "isMainCamera",
            .Editable = true,
        })
        .data<&MCameraComponent::aspectRatio>("aspectRatio"_hs)
        .custom<Info>(Info{
            .DisplayName = "aspectRatio",
            .Editable = true,
        })
        .data<&MCameraComponent::fovX>("fovX"_hs)
        .custom<Info>(Info{
            .DisplayName = "fov",
            .Editable = true,
        })
        .data<&MCameraComponent::fovY>("fovY"_hs)
        .custom<Info>(Info{
            .DisplayName = "fovY",
            .Editable = true,
        })
        .data<&MCameraComponent::zoom>("zoom"_hs)
        .custom<Info>(Info{
            .DisplayName = "zoom",
            .Editable = true,
        });
    entt::meta_factory<MMeshComponent>()
        .type("MMeshComponent"_hs)
        .custom<Info>(Info{
            .DisplayName = "MeshComponent",
        })
        .base<MComponent>()
        .data<&MMeshComponent::meshID>("meshID"_hs)
        .custom<Info>(Info{
            .DisplayName = "Mesh ID",
            .Editable = true,
        });
    entt::meta_factory<MMaterialComponent>()
        .type("MMaterialComponent"_hs)
        .custom<Info>(Info{
            .DisplayName = "MaterialComponent",
        })
        .base<MComponent>()
        .data<&MMaterialComponent::materialID>("materialID"_hs)
        .custom<Info>(Info{
            .DisplayName = "Material ID",
            .Editable = true,
        });
}
static void RegisterMeta()
{
    RegisterAsset();
    RegisterSetting();
    RegisterComponent();
}
