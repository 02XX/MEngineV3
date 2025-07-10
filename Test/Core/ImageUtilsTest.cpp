
#include "ImageUtil.hpp"
#include "gtest/gtest.h"
#include <gtest/gtest.h>

class ImageUtilTest : public ::testing::Test
{
  protected:
    std::filesystem::path testImagePath = "Test/Data/test.png";
    std::filesystem::path outputImagePath = "Test/Data/output_test.png";
    void SetUp() override
    {
        // Setup code if needed
    }
    void TearDown() override
    {
        // Cleanup code if needed
    }
};
TEST_F(ImageUtilTest, LoadAndSave)
{
    stbi_set_flip_vertically_on_load(true);
    int width, height, channels;
    auto data = stbi_load(testImagePath.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);
    channels = 4;
    GTEST_LOG_(INFO) << "Image loaded: " << testImagePath.string() << ", Width: " << width << ", Height: " << height
                     << ", Channels: " << channels;
    stbi_write_png(outputImagePath.string().c_str(), width, height, channels, data, width * channels);
    stbi_image_free(data);
}