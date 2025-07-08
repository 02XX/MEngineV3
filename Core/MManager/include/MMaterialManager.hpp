#pragma once
#include "IMMaterialManager.hpp"
#include "IMPipelineManager.hpp"
#include "IMTextureManager.hpp"
#include "MManager.hpp"
#include "MPipeline.hpp"
#include <memory>

namespace MEngine::Core::Manager
{
template <std::derived_from<MMaterial> TAsset, std::derived_from<MMaterialSetting> TSetting>
class MMaterialManager : public MManager<TAsset, TSetting>, public virtual IMMaterialManager<TAsset, TSetting>
{
  protected:
    std::shared_ptr<IMPipelineManager> mPipelineManager;
    std::shared_ptr<IMTextureManager> mTextureManager;

  public:
    MMaterialManager(std::shared_ptr<VulkanContext> vulkanContext, std::shared_ptr<IUUIDGenerator> uuidGenerator,
                     std::shared_ptr<IMPipelineManager> pipelineManager,
                     std::shared_ptr<IMTextureManager> textureManager)
        : MManager<TAsset, TSetting>(vulkanContext, uuidGenerator), mPipelineManager(pipelineManager),
          mTextureManager(textureManager)
    {
    }
    ~MMaterialManager() override = default;
};
} // namespace MEngine::Core::Manager