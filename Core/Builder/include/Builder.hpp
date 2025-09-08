#pragma once
#include "IBuilder.hpp"

namespace MEngine::Core
{
template <class TAsset> class Builder : IBuilder<TAsset>
{
  public:
    virtual ~Builder() = default;
};
} // namespace MEngine::Core