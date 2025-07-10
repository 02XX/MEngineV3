#pragma once
#include "IMTextureManager.hpp"
#include "IUUIDGenerator.hpp"
#include "MManager.hpp"
#include "MTexture.hpp"
#include "VulkanContext.hpp"
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

using namespace MEngine::Core::Asset;
namespace MEngine::Core::Manager
{
class MTextureManager final : public MManager<MTexture, MTextureSetting>, public IMTextureManager
{
  private:
    vk::UniqueCommandBuffer mCommandBuffer;
    vk::UniqueFence mFence;
    std::unordered_map<DefaultTextureType, std::shared_ptr<MTexture>> mDefaultTextures;

  public:
    MTextureManager(std::shared_ptr<VulkanContext> vulkanContext, std::shared_ptr<IUUIDGenerator> uuidGenerator);
    ~MTextureManager() override = default;
    std::shared_ptr<MTexture> Create(const MTextureSetting &setting, const std::string &name = "New Texture") override;
    void Update(std::shared_ptr<MTexture> texture) override;
    // void Write(std::shared_ptr<MTexture> texture, const std::filesystem::path &path) override;
    void Write(std::shared_ptr<MTexture> texture, const std::vector<uint8_t> &data, const TextureSize &size) override;
    static vk::ImageType TextureTypeToImageType(vk::ImageViewType type);
    static vk::ImageUsageFlags PickImageUsage(const MTextureSetting &setting);
    static vk::ImageCreateFlags PickImageFlags(const MTextureSetting &setting);
    static vk::ImageAspectFlags GuessImageAspectFlags(vk::Format format);
    void CreateDefault() override;
    void CreateVulkanResources(std::shared_ptr<MTexture> asset) override;
    std::shared_ptr<MTexture> CreateWhiteTexture() override;
    std::shared_ptr<MTexture> CreateBlackTexture() override;
    std::shared_ptr<MTexture> CreateNormalTexture() override;
    std::shared_ptr<MTexture> CreateEmissiveTexture() override;
    std::shared_ptr<MTexture> CreateAlbedoTexture() override;
    std::shared_ptr<MTexture> CreateARMTexture() override;
    std::shared_ptr<MTexture> GetDefaultTexture(DefaultTextureType type) const override;

    inline std::vector<uint8_t> GetWhiteData() const
    {
        return std::vector<uint8_t>(4, 255);
    }
    inline std::vector<uint8_t> GetBlackData() const
    {
        return std::vector<uint8_t>(4, 0);
    }
    inline std::vector<uint8_t> GetNormalData() const
    {
        return std::vector<uint8_t>{255, 255, 255, 255};
    }
    inline std::vector<uint8_t> GetEmissiveData() const
    {
        return std::vector<uint8_t>{0, 0, 0, 255};
    }
    inline std::vector<uint8_t> GetAlbedoData() const
    {
        return std::vector<uint8_t>{255, 255, 255, 255};
    }
    inline std::vector<uint8_t> GetARMData() const
    {
        return std::vector<uint8_t>{255, 255, 255, 255};
    }
    // 棋盘格
    inline std::vector<uint8_t> GetCheckerboardData() const
    {
        std::vector<uint8_t> checkerboardData;
        for (int y = 0; y < 16; ++y)
        {
            for (int x = 0; x < 16; ++x)
            {
                uint8_t color = (x + y) % 2 == 0 ? 255 : 0;
                checkerboardData.insert(checkerboardData.end(), {color, color, color, 255});
            }
        }
        return checkerboardData;
    }
};

} // namespace MEngine::Core::Manager