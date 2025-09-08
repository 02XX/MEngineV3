#pragma once
#include <memory>
namespace MEngine::Core
{
template <class TAsset> class IBuilder
{
  public:
    virtual ~IBuilder() = default;
    virtual void Reset() = 0;
    virtual std::unique_ptr<TAsset> Build() = 0;
};
} // namespace MEngine::Core