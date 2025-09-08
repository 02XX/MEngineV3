#pragma once
#include "Asset.hpp"
#include <concepts>
#include <memory>
#include <vector>

namespace MEngine::Core
{
/**
 * @brief 管理资源，提供增删改查接口
 *
 * @tparam TAsset
 */
template <std::derived_from<Asset> TAsset> class IManager
{
  public:
    virtual ~IManager() = default;
    virtual void Add(std::shared_ptr<TAsset> asset) = 0;
    virtual std::shared_ptr<TAsset> Get(const UUID &id) const = 0;
    virtual std::vector<std::shared_ptr<TAsset>> GetAll() const = 0;
    // virtual void Update(const UUID &id, const TAsset &asset) = 0;
    virtual void Remove(const UUID &id) = 0;
};
} // namespace MEngine::Core