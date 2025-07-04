#pragma once
#include "MAsset.hpp"
#include "MAssetSetting.hpp"
#include <concepts>
#include <memory>

using namespace MEngine::Core::Asset;
namespace MEngine::Core::Manager
{
template <std::derived_from<MAsset> TAsset, std::derived_from<MAssetSetting> TSetting> class IMManager
{
  public:
    virtual ~IMManager() = default;
    /**
     * @brief 根据设置创建资产
     *
     * @return std::shared_ptr<MAsset>
     */
    virtual std::shared_ptr<TAsset> Create(const TSetting &setting) = 0;
};
} // namespace MEngine::Core::Manager