#pragma once
#include "IMTextureManager.hpp"
#include "IUUIDGenerator.hpp"
#include "MManager.hpp"
#include "MTexture.hpp"
#include "VulkanContext.hpp"
#include <memory>

using namespace MEngine::Core::Asset;
namespace MEngine::Core::Manager
{
class MTextureManager final : public MManager<MTexture, MTextureSetting>, public IMTextureManager
{
  private:
    vk::UniqueCommandBuffer mCommandBuffer;
    vk::UniqueFence mFence;

  public:
    MTextureManager(std::shared_ptr<VulkanContext> vulkanContext, std::shared_ptr<IUUIDGenerator> uuidGenerator);
    ~MTextureManager() override = default;
    std::shared_ptr<MTexture> Create(const MTextureSetting &setting) override;
    void Write(std::shared_ptr<MTexture> texture, const std::filesystem::path &path) override;
    void Write(std::shared_ptr<MTexture> texture, const std::vector<uint8_t> &data, const TextureSize &size) override;
    static vk::ImageType TextureTypeToImageType(vk::ImageViewType type);
    static vk::ImageUsageFlags PickImageUsage(const MTextureSetting &setting);
    static vk::ImageCreateFlags PickImageFlags(const MTextureSetting &setting);
    static vk::ImageAspectFlags GuessImageAspectFlags(vk::Format format);
};

} // namespace MEngine::Core::Manager