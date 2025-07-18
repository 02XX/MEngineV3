#pragma once
#include "IMManager.hpp"
#include "MTexture.hpp"
#include <cstdint>
#include <memory>
#include <vulkan/vulkan_enums.hpp>

using namespace MEngine::Core::Asset;
namespace MEngine::Core::Manager
{

enum class DefaultTextureType
{
    Magenta, // Used for missing textures in the editor
    White,
    Black,
    Normal,
    Emissive,
    Albedo,
    ARM,
    EnvironmentMap,
    IrradianceMap,
    BRDFLUT
};
class IMTextureManager : public virtual IMManager<MTexture>
{
  public:
    ~IMTextureManager() override = default;
    virtual std::shared_ptr<MTexture> Create(const std::string &name, TextureSize size,
                                             const std::vector<uint8_t> &imageData, const MTextureSetting &setting) = 0;
    virtual void Write(std::shared_ptr<MTexture> texture) = 0;
    virtual std::shared_ptr<MTexture> CreateWhiteTexture() = 0;
    virtual std::shared_ptr<MTexture> CreateBlackTexture() = 0;
    virtual std::shared_ptr<MTexture> CreateMagentaTexture() = 0;
    virtual std::shared_ptr<MTexture> CreateNormalTexture(uint32_t width, uint32_t height) = 0;
    virtual std::shared_ptr<MTexture> CreateEmissiveTexture(uint32_t width, uint32_t height) = 0;
    virtual std::shared_ptr<MTexture> CreateAlbedoTexture(uint32_t width, uint32_t height) = 0;
    virtual std::shared_ptr<MTexture> CreateARMTexture(uint32_t width, uint32_t height) = 0;
    virtual std::shared_ptr<MTexture> CreateEnvironmentMap() = 0;
    virtual std::shared_ptr<MTexture> CreateIrradianceMap() = 0;
    virtual std::shared_ptr<MTexture> CreateBRDFLUT() = 0;
    virtual std::shared_ptr<MTexture> GetDefaultTexture(DefaultTextureType type) const = 0;
    virtual std::shared_ptr<MTexture> CreateColorAttachment(uint32_t width, uint32_t height) = 0;
    virtual std::shared_ptr<MTexture> CreateDepthStencilAttachment(uint32_t width, uint32_t height) = 0;
    virtual std::vector<uint8_t> GetWhiteData() const = 0;
    virtual std::vector<uint8_t> GetBlackData() const = 0;
    virtual std::vector<uint8_t> GetNormalData() const = 0;
    virtual std::vector<uint8_t> GetEmissiveData() const = 0;
    virtual std::vector<uint8_t> GetAlbedoData() const = 0;
    virtual std::vector<uint8_t> GetARMData() const = 0;
    virtual std::vector<uint8_t> GetMagentaData() const = 0;
    virtual std::vector<uint8_t> GetCheckerboardData() const = 0;
};
} // namespace MEngine::Core::Manager