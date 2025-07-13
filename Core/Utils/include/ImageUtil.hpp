#pragma once

#include <filesystem>
#include <ktx.h>
#include <stb_image.h>
#include <stb_image_write.h>
#include <tuple>
#include <vector>
#include <vulkan/vulkan.hpp>

#include <ktxvulkan.h>

namespace MEngine::Core::Utils
{
class ImageUtil
{
  public:
    static std::tuple<int, int, int, std::vector<uint8_t>> LoadImage(const std::filesystem::path &path);
    static std::tuple<int, int, int, std::vector<uint8_t>> LoadHDRImage(const std::filesystem::path &path);
};
} // namespace MEngine::Core::Utils