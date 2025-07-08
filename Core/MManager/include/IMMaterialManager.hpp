#pragma once
#include "IMManager.hpp"
#include "MMaterial.hpp"
using namespace MEngine::Core::Asset;
namespace MEngine::Core::Manager
{
template <std::derived_from<MMaterial> TAsset, std::derived_from<MMaterialSetting> TSetting>
class IMMaterialManager : public virtual IMManager<TAsset, TSetting>
{
  public:
    ~IMMaterialManager() override = default;
    virtual void Write(std::shared_ptr<TAsset> material) = 0;
};
} // namespace MEngine::Core::Manager