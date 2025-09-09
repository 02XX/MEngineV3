#pragma once
#include "UUID.hpp"
#include "UUIDGenerator.hpp"
#include <string>

namespace MEngine::Core
{
enum class AssetType
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
enum class AssetState
{
    Unloaded,
    Loaded
};
class AssetSetting
{
  public:
    virtual ~AssetSetting() = default;
};
class Asset
{
  protected:
    UUID mID{};
    std::string mName{"Unnamed"};
    AssetState mState = AssetState::Unloaded;
    AssetType mType = AssetType::Unknown;
    Asset() : mID(UUIDGenerator::Instance().Create()), mName("Unnamed")
    {
    }

  public:
    virtual ~Asset() = default;
    virtual inline const UUID &GetID() const
    {
        return mID;
    }
    virtual inline void SetID(const UUID &id)
    {
        mID = std::move(id);
    }
    virtual inline AssetType GetType() const
    {
        return mType;
    }
    virtual inline AssetState GetState() const
    {
        return mState;
    }
    virtual inline void SetState(AssetState state)
    {
        mState = state;
    }
    virtual inline void SetType(AssetType type)
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
} // namespace MEngine::Core