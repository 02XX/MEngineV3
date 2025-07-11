#pragma once
#include "MAsset.hpp"

#include "MMaterial.hpp"
#include "MMesh.hpp"
#include "MModel.hpp"
#include "MPBRMaterial.hpp"
#include "MPipeline.hpp"

#include "MTexture.hpp"

#include "UUID.hpp"
#include "Vertex.hpp"
#include <cstdint>
#include <glm/ext/matrix_float4x4.hpp>
#include <magic_enum/magic_enum.hpp>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;
using namespace MEngine::Core::Asset;
namespace nlohmann
{
// Basic types
template <> struct adl_serializer<UUID>
{
    static void to_json(json &j, const UUID &p)
    {
        j = p.ToString();
    }
    static void from_json(const json &j, UUID &p)
    {
        p = UUID(j.get<std::string>());
    }
};
template <> struct adl_serializer<glm::vec3>
{
    static void to_json(json &j, const glm::vec3 &vec)
    {
        j = {vec.x, vec.y, vec.z};
    }
    static void from_json(const json &j, glm::vec3 &vec)
    {
        vec.x = j.at(0).get<float>();
        vec.y = j.at(1).get<float>();
        vec.z = j.at(2).get<float>();
    }
};
template <> struct adl_serializer<glm::vec2>
{
    static void to_json(json &j, const glm::vec2 &vec)
    {
        j = {vec.x, vec.y};
    }
    static void from_json(const json &j, glm::vec2 &vec)
    {
        vec.x = j.at(0).get<float>();
        vec.y = j.at(1).get<float>();
    }
};
template <> struct adl_serializer<Vertex>
{
    static void to_json(json &j, const Vertex &asset)
    {
        j["position"] = asset.position;
        j["normal"] = asset.normal;
        j["texCoords"] = asset.texCoords;
    }
    static void from_json(const json &j, Vertex &asset)
    {
        asset.position = j["position"].get<glm::vec3>();
        asset.normal = j["normal"].get<glm::vec3>();
        asset.texCoords = j["texCoords"].get<glm::vec2>();
    }
};
template <> struct adl_serializer<glm::mat4>
{
    static void to_json(json &j, const glm::mat4 &mat)
    {
        j = {mat[0][0], mat[0][1], mat[0][2], mat[0][3], mat[1][0], mat[1][1], mat[1][2], mat[1][3],
             mat[2][0], mat[2][1], mat[2][2], mat[2][3], mat[3][0], mat[3][1], mat[3][2], mat[3][3]};
    }
    static void from_json(const json &j, glm::mat4 &mat)
    {
        mat = glm::mat4(j.at(0).get<float>(), j.at(1).get<float>(), j.at(2).get<float>(), j.at(3).get<float>(),
                        j.at(4).get<float>(), j.at(5).get<float>(), j.at(6).get<float>(), j.at(7).get<float>(),
                        j.at(8).get<float>(), j.at(9).get<float>(), j.at(10).get<float>(), j.at(11).get<float>(),
                        j.at(12).get<float>(), j.at(13).get<float>(), j.at(14).get<float>(), j.at(15).get<float>());
    }
};
template <> struct adl_serializer<TextureSize>
{
    static void to_json(json &j, const TextureSize &size)
    {
        j = {size.width, size.height, size.channels};
    }
    static void from_json(const json &j, TextureSize &size)
    {
        size.width = j.at(0).get<uint32_t>();
        size.height = j.at(1).get<uint32_t>();
        size.channels = j.at(2).get<uint32_t>();
    }
};
// settings
template <> struct adl_serializer<MAssetSetting>
{
    static void to_json(json &j, const MAssetSetting &setting)
    {
    }
    static void from_json(const json &j, MAssetSetting &setting)
    {
    }
};
template <> struct adl_serializer<MTextureSetting>
{
    static void to_json(json &j, const MTextureSetting &setting)
    {
        j = static_cast<const MAssetSetting &>(setting);
        // 序列化基础属性
        j["mipmapLevels"] = setting.mipmapLevels;
        j["arrayLayers"] = setting.arrayLayers;
        j["sampleCount"] = magic_enum::enum_name(setting.sampleCount);

        // 序列化枚举类型属性
        j["ImageType"] = magic_enum::enum_name(setting.ImageType);
        j["format"] = magic_enum::enum_name(setting.format);
        j["addressModeU"] = magic_enum::enum_name(setting.addressModeU);
        j["addressModeV"] = magic_enum::enum_name(setting.addressModeV);
        j["addressModeW"] = magic_enum::enum_name(setting.addressModeW);
        j["minFilter"] = magic_enum::enum_name(setting.minFilter);
        j["magFilter"] = magic_enum::enum_name(setting.magFilter);
        j["mipmapMode"] = magic_enum::enum_name(setting.mipmapMode);
        j["borderColor"] = magic_enum::enum_name(setting.borderColor);
        j["compareOp"] = magic_enum::enum_name(setting.compareOp);

        // 序列化布尔标志
        j["isShaderResource"] = setting.isShaderResource;
        j["isRenderTarget"] = setting.isRenderTarget;
        j["isUAV"] = setting.isUAV;
        j["isShadingRateSurface"] = setting.isShadingRateSurface;
        j["isTypeless"] = setting.isTypeless;
        j["isTiled"] = setting.isTiled;
        j["compareEnable"] = setting.compareEnable;
        j["anisotropyEnable"] = setting.anisotropyEnable;
        j["unnormalizedCoordinates"] = setting.unnormalizedCoordinates;

        // 序列化浮点属性
        j["mipLodBias"] = setting.mipLodBias;
        j["minLod"] = setting.minLod;
        j["maxLod"] = setting.maxLod;
        j["maxAnisotropy"] = setting.maxAnisotropy;
    }
    static void from_json(const json &j, MTextureSetting &setting)
    {
        j.get_to<MAssetSetting>(setting);
        // 反序列化基础属性
        setting.mipmapLevels = j["mipmapLevels"].get<uint32_t>();
        setting.arrayLayers = j["arrayLayers"].get<uint32_t>();
        auto sampleCountStr = j["sampleCount"].get<std::string>();
        setting.sampleCount =
            magic_enum::enum_cast<vk::SampleCountFlagBits>(sampleCountStr).value_or(vk::SampleCountFlagBits::e1);
        // 反序列化枚举类型属性
        auto imageTypeStr = j["ImageType"].get<std::string>();
        setting.ImageType = magic_enum::enum_cast<vk::ImageViewType>(imageTypeStr).value_or(vk::ImageViewType::e2D);
        auto formatStr = j["format"].get<std::string>();
        setting.format = magic_enum::enum_cast<vk::Format>(formatStr).value_or(vk::Format::eR8G8B8A8Unorm);
        auto addressModeUStr = j["addressModeU"].get<std::string>();
        setting.addressModeU =
            magic_enum::enum_cast<vk::SamplerAddressMode>(addressModeUStr).value_or(vk::SamplerAddressMode::eRepeat);
        auto addressModeVStr = j["addressModeV"].get<std::string>();
        setting.addressModeV =
            magic_enum::enum_cast<vk::SamplerAddressMode>(addressModeVStr).value_or(vk::SamplerAddressMode::eRepeat);
        auto addressModeWStr = j["addressModeW"].get<std::string>();
        setting.addressModeW =
            magic_enum::enum_cast<vk::SamplerAddressMode>(addressModeWStr).value_or(vk::SamplerAddressMode::eRepeat);
        auto minFilterStr = j["minFilter"].get<std::string>();
        setting.minFilter = magic_enum::enum_cast<vk::Filter>(minFilterStr).value_or(vk::Filter::eLinear);
        auto magFilterStr = j["magFilter"].get<std::string>();
        setting.magFilter = magic_enum::enum_cast<vk::Filter>(magFilterStr).value_or(vk::Filter::eLinear);
        auto mipmapModeStr = j["mipmapMode"].get<std::string>();
        setting.mipmapMode =
            magic_enum::enum_cast<vk::SamplerMipmapMode>(mipmapModeStr).value_or(vk::SamplerMipmapMode::eLinear);
        auto borderColorStr = j["borderColor"].get<std::string>();
        setting.borderColor =
            magic_enum::enum_cast<vk::BorderColor>(borderColorStr).value_or(vk::BorderColor::eFloatOpaqueBlack);
        auto compareOpStr = j["compareOp"].get<std::string>();
        setting.compareOp = magic_enum::enum_cast<vk::CompareOp>(compareOpStr).value_or(vk::CompareOp::eAlways);
        // 反序列化布尔标志
        setting.isShaderResource = j["isShaderResource"].get<bool>();
        setting.isRenderTarget = j["isRenderTarget"].get<bool>();
        setting.isUAV = j["isUAV"].get<bool>();
        setting.isShadingRateSurface = j["isShadingRateSurface"].get<bool>();
        setting.isTypeless = j["isTypeless"].get<bool>();
        setting.isTiled = j["isTiled"].get<bool>();
        setting.compareEnable = j["compareEnable"].get<bool>();
        setting.anisotropyEnable = j["anisotropyEnable"].get<bool>();
        setting.unnormalizedCoordinates = j["unnormalizedCoordinates"].get<vk::Bool32>();
    }
};
template <> struct adl_serializer<MPipelineSetting>
{
    static void to_json(json &j, const MPipelineSetting &setting)
    {
        j = static_cast<const MAssetSetting &>(setting);
        j["VertexShaderPath"] = setting.VertexShaderPath.string();
        j["FragmentShaderPath"] = setting.FragmentShaderPath.string();
        j["RenderPassType"] = magic_enum::enum_name(setting.RenderPassType);
        // 光栅化状态
        j["DepthClampEnable"] = setting.DepthClampEnable;
        j["RasterizerDiscardEnable"] = setting.RasterizerDiscardEnable;
        j["PolygonMode"] = magic_enum::enum_name(setting.PolygonMode);
        j["CullMode"] = magic_enum::enum_name(setting.CullMode);
        j["FrontFace"] = magic_enum::enum_name(setting.FrontFace);
        j["DepthBiasEnable"] = setting.DepthBiasEnable;
        j["LineWidth"] = setting.LineWidth;
        // 多重采样
        j["MultisamplingEnable"] = setting.MultisamplingEnable;
        j["SampleCount"] = magic_enum::enum_name(setting.SampleCount);
        j["MinSampleShading"] = setting.MinSampleShading;
        j["AlphaToCoverageEnable"] = setting.AlphaToCoverageEnable;
        j["AlphaToOneEnable"] = setting.AlphaToOneEnable;
        // 深度模板测试
        j["DepthTestEnable"] = setting.DepthTestEnable;
        j["DepthWriteEnable"] = setting.DepthWriteEnable;
        j["DepthCompareOp"] = magic_enum::enum_name(setting.DepthCompareOp);
        j["DepthBoundsTestEnable"] = setting.DepthBoundsTestEnable;
        j["MinDepthBounds"] = setting.MinDepthBounds;
        j["MaxDepthBounds"] = setting.MaxDepthBounds;
        j["StencilTestEnable"] = setting.StencilTestEnable;
        // 颜色混合状态
        j["ColorBlendingEnable"] = setting.ColorBlendingEnable;
        j["LogicOpEnable"] = setting.LogicOpEnable;
        j["LogicOp"] = magic_enum::enum_name(setting.LogicOp);
    }
    static void from_json(const json &j, MPipelineSetting &setting)
    {
        j.get_to<MAssetSetting>(setting);
        setting.VertexShaderPath = j["VertexShaderPath"].get<std::filesystem::path>();
        setting.FragmentShaderPath = j["FragmentShaderPath"].get<std::filesystem::path>();
        auto renderPassTypeStr = j["RenderPassType"].get<std::string>();
        setting.RenderPassType =
            magic_enum::enum_cast<RenderPassType>(renderPassTypeStr).value_or(RenderPassType::ForwardComposition);
        // 光栅化状态
        setting.DepthClampEnable = j["DepthClampEnable"].get<bool>();
        setting.RasterizerDiscardEnable = j["RasterizerDiscardEnable"].get<bool>();
        auto polygonModeStr = j["PolygonMode"].get<std::string>();
        setting.PolygonMode = magic_enum::enum_cast<vk::PolygonMode>(polygonModeStr).value_or(vk::PolygonMode::eFill);
        setting.LineWidth = j["LineWidth"].get<float>();
        auto cullModeStr = j["CullMode"].get<std::string>();
        setting.CullMode =
            magic_enum::enum_cast<vk::CullModeFlagBits>(cullModeStr).value_or(vk::CullModeFlagBits::eBack);
        setting.FrontFace =
            magic_enum::enum_cast<vk::FrontFace>(j["FrontFace"].get<std::string>()).value_or(vk::FrontFace::eClockwise);
        setting.DepthBiasEnable = j["DepthBiasEnable"].get<bool>();
        // 多重采样
        setting.MultisamplingEnable = j["MultisamplingEnable"].get<bool>();
        auto sampleCountStr = j["SampleCount"].get<std::string>();
        setting.SampleCount =
            magic_enum::enum_cast<vk::SampleCountFlagBits>(sampleCountStr).value_or(vk::SampleCountFlagBits::e1);
        setting.MinSampleShading = j["MinSampleShading"].get<float>();
        setting.AlphaToCoverageEnable = j["AlphaToCoverageEnable"].get<bool>();
        setting.AlphaToOneEnable = j["AlphaToOneEnable"].get<bool>();
        // 深度模板测试
        setting.DepthTestEnable = j["DepthTestEnable"].get<bool>();
        setting.DepthWriteEnable = j["DepthWriteEnable"].get<bool>();
        auto depthCompareOpStr = j["DepthCompareOp"].get<std::string>();
        setting.DepthCompareOp =
            magic_enum::enum_cast<vk::CompareOp>(depthCompareOpStr).value_or(vk::CompareOp::eLessOrEqual);
        setting.DepthBoundsTestEnable = j["DepthBoundsTestEnable"].get<bool>();
        setting.MinDepthBounds = j["MinDepthBounds"].get<float>();
        setting.MaxDepthBounds = j["MaxDepthBounds"].get<float>();
        setting.StencilTestEnable = j["StencilTestEnable"].get<bool>();
        // 颜色混合状态
        setting.ColorBlendingEnable = j["ColorBlendingEnable"].get<bool>();
        setting.LogicOpEnable = j["LogicOpEnable"].get<bool>();
        auto logicOpStr = j["LogicOp"].get<std::string>();
        setting.LogicOp = magic_enum::enum_cast<vk::LogicOp>(logicOpStr).value_or(vk::LogicOp::eCopy);
    }
};
template <> struct adl_serializer<MMeshSetting>
{
    static void to_json(json &j, const MMeshSetting &setting)
    {
        j = static_cast<const MAssetSetting &>(setting);
    }
    static void from_json(const json &j, MMeshSetting &setting)
    {
        j.get_to<MAssetSetting>(setting);
    }
};
template <> struct adl_serializer<MMaterialSetting>
{
    static void to_json(json &j, const MMaterialSetting &setting)
    {
        j = static_cast<const MAssetSetting &>(setting);
    }
    static void from_json(const json &j, MMaterialSetting &setting)
    {
        j.get_to<MAssetSetting>(setting);
    }
};
template <> struct adl_serializer<MPBRMaterialSetting>
{
    static void to_json(json &j, const MPBRMaterialSetting &setting)
    {
        j = static_cast<const MMaterialSetting &>(setting);
    }
    static void from_json(const json &j, MPBRMaterialSetting &setting)
    {
        j.get_to<MMaterialSetting>(setting);
    }
};
template <> struct adl_serializer<MModelSetting>
{
    static void to_json(json &j, const MModelSetting &setting)
    {
        j = static_cast<const MAssetSetting &>(setting);
    }
    static void from_json(const json &j, MModelSetting &setting)
    {
        j.get_to<MAssetSetting>(setting);
    }
};
// MAsset
template <> struct adl_serializer<MAsset>
{
    static void to_json(json &j, const MAsset &p)
    {
        j["id"] = p.GetID();
        j["type"] = magic_enum::enum_name(p.GetType());
        j["state"] = magic_enum::enum_name(p.GetState());
        j["name"] = p.GetName();
    }

    static void from_json(const json &j, MAsset &p)
    {
        auto id = j["id"].get<UUID>();
        auto type = magic_enum::enum_cast<MAssetType>(j["type"].get<std::string>());
        auto state = magic_enum::enum_cast<MAssetState>(j["state"].get<std::string>());
        p.SetID(id);
        if (type.has_value())
        {
            p.SetType(type.value());
        }
        else
        {
            throw std::runtime_error("Unknown MAssetType: " + j["type"].get<std::string>());
        }
        if (state.has_value())
        {
            p.SetState(state.value());
        }
        else
        {
            throw std::runtime_error("Unknown MAssetState: " + j["state"].get<std::string>());
        }
        p.SetName(j["name"].get<std::string>());
    }
};
template <> struct adl_serializer<MTexture>
{
    static void to_json(json &j, const MTexture &asset)
    {
        j = static_cast<const MAsset &>(asset);
        j["data"] = json::binary(asset.mImageData);
        j["size"] = asset.mSize;
        j["setting"] = asset.mSetting;
    }
    static void from_json(const json &j, MTexture &asset)
    {
        j.get_to<MAsset>(asset);
        asset.mImageData = j["data"].get_binary();
        asset.mSetting = j["setting"].get<MTextureSetting>();
        asset.mSize = j.at("size").get<TextureSize>();
    }
};
template <> struct adl_serializer<MPipeline>
{
    static void to_json(json &j, const MPipeline &pipeline)
    {
        j = static_cast<const MAsset &>(pipeline);
        j["setting"] = pipeline.mSetting;
    }
    static void from_json(const json &j, MPipeline &pipeline)
    {
        j.get_to<MAsset>(pipeline);
        pipeline.mSetting = j["setting"].get<MPipelineSetting>();
    }
};

template <> struct adl_serializer<MMesh>
{
    static void to_json(json &j, const MMesh &asset)
    {
        j = static_cast<const MAsset &>(asset);
        j["vertices"] = asset.mVertices;
        j["indices"] = asset.mIndices;
        j["setting"] = asset.mSetting;
    }
    static void from_json(const json &j, MMesh &asset)
    {
        j.get_to<MAsset>(asset);
        asset.mVertices = j["vertices"].get<std::vector<Vertex>>();
        asset.mIndices = j["indices"].get<std::vector<uint32_t>>();
        asset.mSetting = j["setting"].get<MMeshSetting>();
    }
};
template <> struct adl_serializer<MMaterial>
{
    static void to_json(json &j, const MMaterial &asset)
    {
        j = static_cast<const MAsset &>(asset);
        j["setting"] = asset.mSetting;
        j["materialType"] = magic_enum::enum_name(asset.mMaterialType);
        j["pipelineName"] = asset.mPipelineName;
    }
    static void from_json(const json &j, MMaterial &asset)
    {
        j.get_to<MAsset>(asset);
        asset.mSetting = j["setting"].get<MMaterialSetting>();
        auto materialTypeStr = j["materialType"].get<std::string>();
        asset.mMaterialType = magic_enum::enum_cast<MMaterialType>(materialTypeStr).value_or(MMaterialType::Unknown);
        asset.mPipelineName = j["pipelineName"].get<std::string>();
    }
};
template <> struct adl_serializer<MPBRMaterialProperties>
{
    static void to_json(json &j, const MPBRMaterialProperties &asset)
    {
        j["Albedo"] = asset.Albedo;
        j["Normal"] = asset.Normal;
        j["Metallic"] = asset.Metallic;
        j["Roughness"] = asset.Roughness;
        j["AO"] = asset.AO;
        j["EmissiveIntensity"] = asset.EmissiveIntensity;
    }
    static void from_json(const json &j, MPBRMaterialProperties &asset)
    {
        asset.Albedo = j["Albedo"].get<glm::vec3>();
        asset.Normal = j["Normal"].get<glm::vec3>();
        asset.Metallic = j["Metallic"].get<float>();
        asset.Roughness = j["Roughness"].get<float>();
        asset.AO = j["AO"].get<float>();
        asset.EmissiveIntensity = j["EmissiveIntensity"].get<float>();
    }
};
template <> struct adl_serializer<MPBRTextures>
{
    static void to_json(json &j, const MPBRTextures &asset)
    {
        j["AlbedoID"] = asset.AlbedoID;
        j["NormalID"] = asset.NormalID;
        j["ARMID"] = asset.ARMID;
        j["EmissiveID"] = asset.EmissiveID;
    }
    static void from_json(const json &j, MPBRTextures &asset)
    {
        asset.AlbedoID = j["AlbedoID"].get<UUID>();
        asset.NormalID = j["NormalID"].get<UUID>();
        asset.ARMID = j["ARMID"].get<UUID>();
        asset.EmissiveID = j["EmissiveID"].get<UUID>();
    }
};
template <> struct adl_serializer<MPBRMaterial>
{
    static void to_json(json &j, const MPBRMaterial &asset)
    {
        j = static_cast<const MMaterial &>(asset);
        j["setting"] = asset.mSetting;
        j["properties"] = asset.mProperties;
        j["textures"] = asset.mTextures;
    }
    static void from_json(const json &j, MPBRMaterial &asset)
    {
        j.get_to<MMaterial>(asset);
        asset.mSetting = j["setting"].get<MPBRMaterialSetting>();
        asset.mProperties = j["properties"].get<MPBRMaterialProperties>();
        asset.mTextures = j["textures"].get<MPBRTextures>();
    }
};
template <> struct adl_serializer<Node>
{
    static void to_json(json &j, const Node &node)
    {
        j["name"] = node.Name;
        j["transform"] = node.Transform;
        j["MeshIndex"] = node.MeshIndex;
        j["MaterialIndex"] = node.MaterialIndex;
        j["children"] = json::array();
        for (const auto &child : node.Children)
        {
            json childJson;
            to_json(childJson, *child);
            j["children"].push_back(childJson);
        }
    }
    static void from_json(const json &j, Node &node)
    {
        node.Name = j["name"].get<std::string>();
        node.Transform = j["transform"].get<glm::mat4>();
        node.MeshIndex = j["MeshIndex"].get<uint32_t>();
        node.MaterialIndex = j["MaterialIndex"].get<uint32_t>();
        for (const auto &childJson : j.at("children"))
        {
            auto child = std::make_unique<Node>();
            from_json(childJson, *child);
            node.Children.push_back(std::move(child));
        }
    }
};
template <> struct adl_serializer<MModel>
{
    static void to_json(json &j, const MModel &asset)
    {
        j = static_cast<const MAsset &>(asset);
        j["setting"] = asset.mSetting;
        j["meshIDs"] = asset.mMeshIDs;
        j["materialIDs"] = asset.mMaterialIDs;
        j["rootNode"] = *asset.mRootNode;
        j["meshes"] = json::array();
        for (const auto &mesh : asset.mMeshes)
        {
            j["meshes"].push_back(*mesh);
        }
        j["materials"] = json::array();
        for (const auto &material : asset.mMaterials)
        {
            switch (material->GetMaterialType())
            {
            case MMaterialType::Unknown:
            case MMaterialType::PBR: {
                j["materials"].push_back(*std::dynamic_pointer_cast<MPBRMaterial>(material));
            }
            case MMaterialType::Unlit:
            case MMaterialType::Custom:
                break;
            }
        }
    }
    static void from_json(const json &j, MModel &asset)
    {
        j.get_to<MAsset>(asset);
        asset.mSetting = j["setting"].get<MModelSetting>();
        asset.mMeshIDs = j["meshIDs"].get<std::vector<UUID>>();
        asset.mMaterialIDs = j["materialIDs"].get<std::vector<UUID>>();
        asset.mRootNode = std::make_unique<Node>();
        j["rootNode"].get_to(*asset.mRootNode);
    }
};
} // namespace nlohmann