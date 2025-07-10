#pragma once
#include "UUID.hpp"
#include "VulkanContext.hpp"
#include <filesystem>

using namespace MEngine::Core;
namespace MEngine::Core::Asset
{

enum class MAssetType
{
    Texture,
    Material,
    Mesh,
    Model,
    Shader,
    Animation,
    Scene,
    Audio,
    Font,
    Folder,
    File,
    Script,
    Unknown
};
enum class MAssetState
{
    Unloaded,
    Loaded
};
class MAssetSetting
{
  public:
    virtual ~MAssetSetting() = default;
};
class MAsset
{
  protected:
  protected:
    UUID mID{};
    std::string mName{"Unnamed"};
    std::filesystem::path mPath{};
    MAssetState mState = MAssetState::Unloaded;
    MAssetType mType = MAssetType::Unknown;

  public:
    MAsset(const UUID &id, const std::string &name) : mID(id), mName(name)
    {
    }
    virtual ~MAsset() = default;
    inline const UUID &GetID() const
    {
        return mID;
    }
    inline void SetID(const UUID &id)
    {
        mID = id;
    }
    inline MAssetType GetType() const
    {
        return mType;
    }
    inline MAssetState GetState() const
    {
        return mState;
    }
    inline void SetState(MAssetState state)
    {
        mState = state;
    }
    inline void SetType(MAssetType type)
    {
        mType = type;
    }
    inline const std::filesystem::path &GetPath() const
    {
        return mPath;
    }
    inline void SetPath(const std::filesystem::path &path)
    {
        mPath = path;
    }
    inline const std::string &GetName() const
    {
        return mName;
    }
    inline void SetName(const std::string &name)
    {
        mName = name;
    }
};
} // namespace MEngine::Core::Asset