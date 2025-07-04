#pragma once
#include "UUID.hpp"

namespace MEngine::Core
{
class IUUIDGenerator
{
  public:
    virtual ~IUUIDGenerator() = default;
    virtual UUID Create() = 0;
};
} // namespace MEngine::Core