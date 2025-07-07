#pragma once
#include "MAsset.hpp"

#include "MPipeline.hpp"

#include "MTexture.hpp"

#include "UUID.hpp"
#include <magic_enum/magic_enum.hpp>
#include <nlohmann/json.hpp>

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
        j["width"] = setting.width;
        j["height"] = setting.height;
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
        setting.width = j["width"].get<uint32_t>();
        setting.height = j["height"].get<uint32_t>();
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
// MAsset
template <> struct adl_serializer<MAsset>
{
    static void to_json(json &j, const MAsset &p)
    {
        j["id"] = p.GetID();
        j["type"] = magic_enum::enum_name(p.GetType());
        j["state"] = magic_enum::enum_name(p.GetState());
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
    }
};
template <> struct adl_serializer<MTexture>
{
    static void to_json(json &j, const MTexture &asset)
    {
        j = static_cast<const MAsset &>(asset);
        j["data"] = asset.mImageData;
        j["setting"] = asset.mSetting;
    }
    static void from_json(const json &j, MTexture &asset)
    {
        j.get_to<MAsset>(asset);
        asset.mImageData = j["data"].get<std::vector<uint8_t>>();
        asset.mSetting = j["setting"].get<MTextureSetting>();
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
} // namespace nlohmann