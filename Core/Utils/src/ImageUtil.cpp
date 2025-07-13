#include <cstdint>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include "ImageUtil.hpp"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
namespace MEngine::Core::Utils
{
std::tuple<int, int, int, std::vector<uint8_t>> ImageUtil::LoadImage(const std::filesystem::path &path)
{
    std::vector<uint8_t> imageData;
    stbi_set_unpremultiply_on_load(true);
    int width, height, channels;
    unsigned char *data = stbi_load(path.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);
    if (!data)
    {
        throw std::runtime_error("Failed to load image: " + path.string());
    }
    channels = 4;
    imageData.assign(data, data + width * height * channels);
    stbi_image_free(data);
    return {width, height, channels, imageData};
}
std::tuple<int, int, int, std::vector<uint8_t>> ImageUtil::LoadHDRImage(const std::filesystem::path &path)
{
    std::vector<float> imageData;
    stbi_set_flip_vertically_on_load(true);
    int width, height, channels;
    float *data = stbi_loadf(path.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);
    if (!data)
    {
        throw std::runtime_error("Failed to load HDR image: " + path.string());
    }
    channels = 4;
    imageData.assign(data, data + width * height * channels);
    stbi_image_free(data);
    std::vector<uint8_t> byteData(imageData.size() * sizeof(float));
    std::memcpy(byteData.data(), imageData.data(), byteData.size());
    return {width, height, channels, byteData};
}
} // namespace MEngine::Core::Utils