#define STB_IMAGE_IMPLEMENTATION
#include "ImageUtil.hpp"
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
    imageData.assign(data, data + width * height * channels);
    stbi_image_free(data);
    return {width, height, channels, imageData};
}
} // namespace MEngine::Core::Utils