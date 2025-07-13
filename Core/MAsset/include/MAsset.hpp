#pragma once
#include "UUID.hpp"
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
    MAssetState mState = MAssetState::Unloaded;
    MAssetType mType = MAssetType::Unknown;

  public:
    MAsset(const UUID &id, const std::string &name) : mID(id), mName(name)
    {
    }
    virtual ~MAsset() = default;
    virtual inline const UUID &GetID() const
    {
        return mID;
    }
    virtual inline void SetID(const UUID &id)
    {
        mID = id;
    }
    virtual inline MAssetType GetType() const
    {
        return mType;
    }
    virtual inline MAssetState GetState() const
    {
        return mState;
    }
    virtual inline void SetState(MAssetState state)
    {
        mState = state;
    }
    virtual inline void SetType(MAssetType type)
    {
        mType = type;
    }
    virtual inline const std::string &GetName() const
    {
        return mName;
    }
    virtual inline void SetName(const std::string &name)
    {
        mName = name;
    }
};
} // namespace MEngine::Core::Asset