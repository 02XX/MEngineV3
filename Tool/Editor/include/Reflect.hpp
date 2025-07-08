#include "MAsset.hpp"
#include "MCameraComponent.hpp"
#include "MLightComponent.hpp"
#include "MMaterial.hpp"
#include "MMaterialComponent.hpp"
#include "MMeshComponent.hpp"
#include "MPBRMaterial.hpp"
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
    entt::meta_factory<MMaterial>()
        .base<MAsset>()
        .type("Material"_hs)
        .custom<Info>(Info{
            .DisplayName = "Material",
            .Editable = true,
            .Serializable = true,
        })
        .data<&MMaterial::SetPipeline, &MMaterial::GetPipeline>("Pipeline"_hs)
        .custom<Info>(Info{
            .DisplayName = "Pipeline",
            .Editable = true,
            .Serializable = true,
        });
    entt::meta_factory<MPBRMaterial>()
        .base<MMaterial>()
        .type("PBRMaterial"_hs)
        .custom<Info>(Info{
            .DisplayName = "PBRMaterial",
            .Editable = true,
            .Serializable = true,
        })
        .data<&MPBRMaterial::SetProperties, &MPBRMaterial::GetProperties>("PBRProperties"_hs)
        .custom<Info>(Info{
            .DisplayName = "PBR Properties",
            .Editable = true,
            .Serializable = true,
        })
        .data<&MPBRMaterial::SetTextures, &MPBRMaterial::GetTextures>("PBRTextures"_hs)
        .custom<Info>(Info{
            .DisplayName = "PBR Textures",
            .Editable = true,
            .Serializable = true,
        });
    entt::meta_factory<MPBRMaterialProperties>()
        .type("MPBRMaterialProperties"_hs)
        .custom<Info>(Info{
            .DisplayName = "PBR Material Properties",
            .Editable = true,
            .Serializable = true,
        })
        .data<&MPBRMaterialProperties::Albedo>("Albedo"_hs)
        .custom<Info>(Info{
            .DisplayName = "Albedo",
            .Editable = true,
            .Serializable = true,
        })
        .data<&MPBRMaterialProperties::Normal>("Normal"_hs)
        .custom<Info>(Info{
            .DisplayName = "Normal",
            .Editable = true,
            .Serializable = true,
        })
        .data<&MPBRMaterialProperties::Metallic>("Metallic"_hs)
        .custom<Info>(Info{
            .DisplayName = "Metallic",
            .Editable = true,
            .Serializable = true,
        })
        .data<&MPBRMaterialProperties::Roughness>("Roughness"_hs)
        .custom<Info>(Info{
            .DisplayName = "Roughness",
            .Editable = true,
            .Serializable = true,
        })
        .data<&MPBRMaterialProperties::AO>("AO"_hs)
        .custom<Info>(Info{
            .DisplayName = "Ambient Occlusion",
            .Editable = true,
            .Serializable = true,
        })
        .data<&MPBRMaterialProperties::EmissiveIntensity>("EmissiveIntensity"_hs)
        .custom<Info>(Info{
            .DisplayName = "Emissive Intensity",
            .Editable = true,
            .Serializable = true,
        });
    entt::meta_factory<MPBRTextures>()
        .type("MPBRTextures"_hs)
        .custom<Info>(Info{
            .DisplayName = "PBR Textures",
            .Editable = true,
            .Serializable = true,
        })
        .data<&MPBRTextures::Albedo>("Albedo"_hs)
        .custom<Info>(Info{
            .DisplayName = "Albedo Texture",
            .Editable = true,
            .Serializable = true,
        })
        .data<&MPBRTextures::Normal>("Normal"_hs)
        .custom<Info>(Info{
            .DisplayName = "Normal Texture",
            .Editable = true,
            .Serializable = true,
        })
        .data<&MPBRTextures::ARM>("ARM"_hs)
        .custom<Info>(Info{
            .DisplayName = "ARM Texture",
            .Editable = true,
            .Serializable = true,
        })
        .data<&MPBRTextures::Emissive>("Emissive"_hs)
        .custom<Info>(Info{
            .DisplayName = "Emissive Texture",
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
            .Serializable = true,
        })
        .data<&MCameraComponent::aspectRatio>("aspectRatio"_hs)
        .custom<Info>(Info{
            .DisplayName = "aspectRatio",
            .Editable = true,
            .Serializable = true,
        })
        .data<&MCameraComponent::fovX>("fovX"_hs)
        .custom<Info>(Info{
            .DisplayName = "fov",
            .Editable = true,
            .Serializable = true,
        })
        .data<&MCameraComponent::fovY>("fovY"_hs)
        .custom<Info>(Info{
            .DisplayName = "fovY",
            .Editable = true,
            .Serializable = true,
        })
        .data<&MCameraComponent::zoom>("zoom"_hs)
        .custom<Info>(Info{
            .DisplayName = "zoom",
            .Editable = true,
            .Serializable = true,
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
            .Serializable = true,
        })
        .data<&MMeshComponent::mesh>("mesh"_hs)
        .custom<Info>(Info{
            .DisplayName = "Mesh",
            .Editable = true,
            .Serializable = true,
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
            .Serializable = true,
        })
        .data<&MMaterialComponent::material>("material"_hs)
        .custom<Info>(Info{
            .DisplayName = "Material",
            .Editable = true,
            .Serializable = true,
        });
    entt::meta_factory<MLightComponent>()
        .type("MLightComponent"_hs)
        .custom<Info>(Info{
            .DisplayName = "LightComponent",
        })
        .base<MComponent>()
        .data<&MLightComponent::LightType>("LightType"_hs)
        .custom<Info>(Info{
            .DisplayName = "Light Type",
            .Editable = true,
            .Serializable = true,
        })
        .data<&MLightComponent::Intensity>("Intensity"_hs)
        .custom<Info>(Info{
            .DisplayName = "Intensity",
            .Editable = true,
            .Serializable = true,
        })
        .data<&MLightComponent::Color>("Color"_hs)
        .custom<Info>(Info{
            .DisplayName = "Color",
            .Editable = true,
            .Serializable = true,
        })
        .data<&MLightComponent::Radius>("Radius"_hs)
        .custom<Info>(Info{
            .DisplayName = "Radius",
            .Editable = true,
            .Serializable = true,
        })
        .data<&MLightComponent::InnerConeAngle>("InnerConeAngle"_hs)
        .custom<Info>(Info{
            .DisplayName = "Inner Cone Angle",
            .Editable = true,
            .Serializable = true,
        })
        .data<&MLightComponent::OuterConeAngle>("OuterConeAngle"_hs)
        .custom<Info>(Info{
            .DisplayName = "Outer Cone Angle",
            .Editable = true,
            .Serializable = true,
        });
}
static void RegisterMeta()
{
    RegisterAsset();
    RegisterSetting();
    RegisterComponent();
}
