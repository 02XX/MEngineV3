#pragma once
#include "UUID.hpp"
#include "UUIDGenerator.hpp"
#include <string>

namespace MEngine::Core
{
class Asset
{
  protected:
    UUID mID{};
    std::string mName{"Unnamed"};
    Asset() : mID(UUIDGenerator::Instance().Create()), mName("Unnamed")
    {
    }

  public:
    virtual ~Asset() = default;
    virtual inline const UUID &GetID() const
    {
        return mID;
    }
    virtual inline const std::string &GetName() const
    {
        return mName;
    }
};
} // namespace MEngine::Core