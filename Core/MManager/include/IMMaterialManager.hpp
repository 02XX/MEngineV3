#pragma once
#include "IMManager.hpp"
#include "MMaterial.hpp"
#include <memory>
using namespace MEngine::Core::Asset;
namespace MEngine::Core::Manager
{
template <std::derived_from<MMaterial> TAsset> class IMMaterialManager : public virtual IMManager<TAsset>
{
  public:
    ~IMMaterialManager() override = default;
    virtual void Write(std::shared_ptr<TAsset> material) = 0;
};
} // namespace MEngine::Core::Manager