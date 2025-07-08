#pragma once
#include "MAsset.hpp"

#include <concepts>
#include <memory>
#include <vector>

using namespace MEngine::Core::Asset;
namespace MEngine::Core::Manager
{
class IMManagerBase
{
  public:
    virtual ~IMManagerBase() = default;
};
template <std::derived_from<MAsset> TAsset, std::derived_from<MAssetSetting> TSetting>
class IMManager : public IMManagerBase
{
  public:
    ~IMManager() override = default;
    /**
     * @brief 根据设置创建资产
     *
     * @return std::shared_ptr<MAsset>
     */
    virtual std::shared_ptr<TAsset> Create(const TSetting &setting, const std::string &name) = 0;
    virtual std::shared_ptr<TAsset> Get(const UUID &id) const = 0;
    virtual std::vector<std::shared_ptr<TAsset>> GetAll() const = 0;
    virtual void Update(std::shared_ptr<TAsset> asset) = 0;
    virtual void Remove(const UUID &id) = 0;
    virtual void CreateDefault() = 0;
};
} // namespace MEngine::Core::Manager