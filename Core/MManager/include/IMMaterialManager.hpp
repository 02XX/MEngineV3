#pragma once
#include "IMManager.hpp"
#include "MMaterial.hpp"
using namespace MEngine::Core::Asset;
namespace MEngine::Core::Manager
{
template <std::derived_from<MMaterial> TAsset, std::derived_from<MMaterialSetting> TSetting>
class IMMaterialManager : public IMManager<TAsset, TSetting>
{
  public:
    ~IMMaterialManager() override = default;
};
} // namespace MEngine::Core::Manager