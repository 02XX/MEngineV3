#pragma once
#include "IMManager.hpp"
#include "MTexture.hpp"
#include <memory>

using namespace MEngine::Core::Asset;
namespace MEngine::Core::Manager
{
struct TextureSize
{
    uint32_t width;
    uint32_t height;
    uint32_t channels;
};
class IMTextureManager : public virtual IMManager<MTexture, MTextureSetting>
{
  public:
    ~IMTextureManager() override = default;
    /**
     * @brief 将图片写入到纹理
     *
     * @param texture 目标纹理
     * @param path 图片路径
     */
    virtual void Write(std::shared_ptr<MTexture> texture, const std::filesystem::path &path) = 0;
    /**
     * @brief 将数据写入到纹理
     *
     * @param texture 目标纹理
     * @param data 数据
     * @param extent 纹理尺寸
     */
    virtual void Write(std::shared_ptr<MTexture> texture, const std::vector<uint8_t> &data,
                       const TextureSize &size) = 0;
};
} // namespace MEngine::Core::Manager