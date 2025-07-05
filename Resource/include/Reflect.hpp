#include "MAsset.hpp"
#include "MPipeline.hpp"
#include "MTexture.hpp"
#include <entt/meta/factory.hpp>
using namespace entt::literals;
using namespace MEngine::Core::Asset;
struct Info
{
    std::string DisplayName = "Unknown";
    bool Editable = false;
    bool Serializable = true;
};
static void RegisterAsset()
{
    entt::meta_factory<MAsset>()
        .type("MAsset"_hs)
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
        .type("MTexture"_hs)
        .custom<Info>(Info{
            .DisplayName = "Texture",
            .Editable = true,
            .Serializable = true,
        })
        .data<&MTexture::SetImagePath, &MTexture::GetImagePath>("ImagePath"_hs)
        .custom<Info>(Info{
            .DisplayName = "Image Path",
            .Editable = false,
            .Serializable = true,
        });
    entt::meta_factory<MPipeline>()
        .base<MAsset>()
        .type("MPipeline"_hs)
        .custom<Info>(Info{
            .DisplayName = "Pipeline",
            .Editable = true,
            .Serializable = true,
        });
}
static void RegisterSetting()
{
}
static void RegisterMeta()
{
    RegisterAsset();
    RegisterSetting();
}
