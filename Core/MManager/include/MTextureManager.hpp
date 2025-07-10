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

class MTextureManager final : public MManager<MTexture>, public IMTextureManager
{
  private:
    vk::UniqueCommandBuffer mCommandBuffer;
    vk::UniqueFence mFence;
    std::unordered_map<DefaultTextureType, UUID> mDefaultTextures{
        {DefaultTextureType::Magenta, UUID{"00000000-0000-0000-0000-000000000000"}},
        {DefaultTextureType::White, UUID{"00000000-0000-0000-0000-000000000001"}},
        {DefaultTextureType::Black, UUID{"00000000-0000-0000-0000-000000000002"}},
        {DefaultTextureType::Normal, UUID{"00000000-0000-0000-0000-000000000003"}},
        {DefaultTextureType::Emissive, UUID{"00000000-0000-0000-0000-000000000004"}},
        {DefaultTextureType::Albedo, UUID{"00000000-0000-0000-0000-000000000005"}},
        {DefaultTextureType::ARM, UUID{"00000000-0000-0000-0000-000000000006"}}};

  public:
    MTextureManager(std::shared_ptr<VulkanContext> vulkanContext, std::shared_ptr<IUUIDGenerator> uuidGenerator);
    ~MTextureManager() override = default;
    std::shared_ptr<MTexture> Create(const std::string &name, TextureSize size, const std::vector<uint8_t> &imageData,
                                     const MTextureSetting &setting) override;
    void Update(std::shared_ptr<MTexture> texture) override;
    // void Write(std::shared_ptr<MTexture> texture, const std::filesystem::path &path) override;
    void Write(std::shared_ptr<MTexture> texture) override;
    static vk::ImageType TextureTypeToImageType(vk::ImageViewType type);
    static vk::ImageUsageFlags PickImageUsage(const MTextureSetting &setting);
    static vk::ImageCreateFlags PickImageFlags(const MTextureSetting &setting);
    static vk::ImageAspectFlags GuessImageAspectFlags(vk::Format format);
    void CreateDefault() override;
    void CreateVulkanResources(std::shared_ptr<MTexture> asset) override;
    std::shared_ptr<MTexture> CreateWhiteTexture() override;
    std::shared_ptr<MTexture> CreateBlackTexture() override;
    std::shared_ptr<MTexture> CreateMagentaTexture() override;
    std::shared_ptr<MTexture> CreateNormalTexture() override;
    std::shared_ptr<MTexture> CreateEmissiveTexture() override;
    std::shared_ptr<MTexture> CreateAlbedoTexture() override;
    std::shared_ptr<MTexture> CreateARMTexture() override;
    std::shared_ptr<MTexture> GetDefaultTexture(DefaultTextureType type) const override;
    std::shared_ptr<MTexture> CreateColorAttachment(uint32_t width, uint32_t height) override;
    std::shared_ptr<MTexture> CreateDepthStencilAttachment(uint32_t width, uint32_t height) override;
    inline std::vector<uint8_t> GetWhiteData() const override
    {
        return std::vector<uint8_t>(4, 255);
    }
    inline std::vector<uint8_t> GetBlackData() const override
    {
        return std::vector<uint8_t>(4, 0);
    }
    inline std::vector<uint8_t> GetNormalData() const override
    {
        return std::vector<uint8_t>{255, 255, 255, 255};
    }
    inline std::vector<uint8_t> GetEmissiveData() const override
    {
        return std::vector<uint8_t>{0, 0, 0, 255};
    }
    inline std::vector<uint8_t> GetAlbedoData() const override
    {
        return std::vector<uint8_t>{255, 255, 255, 255};
    }
    inline std::vector<uint8_t> GetARMData() const override
    {
        return std::vector<uint8_t>{255, 255, 255, 255};
    }
    inline std::vector<uint8_t> GetMagentaData() const override
    {
        return std::vector<uint8_t>{255, 0, 255, 255};
    }
    // 棋盘格
    inline std::vector<uint8_t> GetCheckerboardData() const override
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