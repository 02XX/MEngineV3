#pragma once
#include "IMManager.hpp"
#include "IUUIDGenerator.hpp"
#include "Logger.hpp"
#include "UUID.hpp"
#include "VulkanContext.hpp"
#include <concepts>
#include <memory>
#include <ranges>
#include <unordered_map>

using namespace MEngine::Core::Asset;
namespace MEngine::Core::Manager
{
template <std::derived_from<MAsset> TAsset> class MManager : public virtual IMManager<TAsset>
{
  protected:
    // ID
    std::shared_ptr<VulkanContext> mVulkanContext;
    std::shared_ptr<IUUIDGenerator> mUUIDGenerator;

  protected:
    std::unordered_map<UUID, std::shared_ptr<TAsset>> mAssets;

  public:
    MManager(std::shared_ptr<VulkanContext> vulkanContext, std::shared_ptr<IUUIDGenerator> uuidGenerator)
        : mVulkanContext(vulkanContext), mUUIDGenerator(uuidGenerator)
    {
    }
    virtual std::shared_ptr<TAsset> Get(const UUID &id) const override
    {
        if (mAssets.contains(id))
        {
            return mAssets.at(id);
        }
        LogWarn("Asset with ID {} not found", id.ToString());
        return nullptr;
    }
    virtual std::vector<std::shared_ptr<TAsset>> GetAll() const override
    {
        return mAssets | std::views::values | std::ranges::to<std::vector<std::shared_ptr<TAsset>>>();
    }
    virtual void Remove(const UUID &id) override
    {
        if (mAssets.contains(id))
        {
            mAssets.erase(id);
        }
    }
    ~MManager() override = default;
};
} // namespace MEngine::Core::Manager