
#include "ImageUtil.hpp"
#include "gtest/gtest.h"
#include <chrono>
#include <cstdint>
#include <fstream>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <vector>

using json = nlohmann::json;
static int count = 0;
class ABC
{
  public:
    std::string name = "test";
    int age = 0;
    ABC()
    {
        age = ++count;
        GTEST_LOG_(INFO) << "ABC constructor: " << age;
    }
};
class Image
{
  public:
    std::vector<uint8_t> data;
    std::string name;
};
namespace nlohmann
{
template <> struct adl_serializer<ABC>
{
    static void to_json(json &j, const ABC &entity)
    {
        j["name"] = entity.name;
    }

    static void from_json(const json &j, ABC &entity)
    {
        entity.name = j["name"].get<std::string>();
    }
};
template <> struct adl_serializer<Image>
{
    static void to_json(json &j, const Image &image)
    {
        j["name"] = image.name;
        j["data"] = json::binary(image.data);
    }
    static void from_json(const json &j, Image &image)
    {
        image.name = j["name"].get<std::string>();
        image.data = j["data"].get_binary();
    }
};
} // namespace nlohmann

TEST(SerializeTest, serialize)
{
    auto entity = std::make_shared<ABC>();
    nlohmann::json j;
    EXPECT_NO_THROW(j = *entity);
    GTEST_LOG_(INFO) << j.dump(4);
}
TEST(SerializeTest, deserialize)
{
    nlohmann::json j;
    j["name"] = "test";
    auto entity = std::make_shared<ABC>();
    EXPECT_NO_THROW(j.get_to(*entity));
    GTEST_LOG_(INFO) << entity->age;
    EXPECT_EQ(entity->age, 2);
}
TEST(SerializeTest, Basic_Serialize)
{
    auto image = std::make_shared<Image>();
    image->name = "test_image";
    auto &&[W, H, C, data] = MEngine::Core::Utils::ImageUtil::LoadImage("Test/Data/test.png");
    image->data = data;
    // Serialize
    json j = *image;
    auto start = std::chrono::steady_clock::now();
    auto bson = json::to_msgpack(j);

    std::ofstream ofs("Test/Data/test_image.json");
    ofs << j.dump(4);
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    GTEST_LOG_(INFO) << "Serialization took: " << duration.count() << " ms";
    ofs.close();
    // Deserialize
    start = std::chrono::steady_clock::now();
    std::ifstream ifs("Test/Data/test_image.json");
    json j2 = json::parse(ifs);
    end = std::chrono::steady_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    GTEST_LOG_(INFO) << "Deserialization took: " << duration.count() << " ms";
    ifs.close();
}
TEST(SerializeTest, Binary_Serialize)
{
    auto image = std::make_shared<Image>();
    image->name = "test_image";
    auto &&[W, H, C, data] = MEngine::Core::Utils::ImageUtil::LoadImage("Test/Data/test.png");
    image->data = data;
    // Serialize
    json j = *image;
    auto start = std::chrono::steady_clock::now();
    auto bson = json::to_msgpack(j);

    std::ofstream ofs("Test/Data/test_image.bson", std::ios::binary);
    ofs.write(reinterpret_cast<const char *>(bson.data()), bson.size());
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    GTEST_LOG_(INFO) << "Serialization took: " << duration.count() << " ms";
    ofs.close();
    // Deserialize
    start = std::chrono::steady_clock::now();
    std::ifstream ifs("Test/Data/test_image.bson", std::ios::binary);
    json j2 = json::from_msgpack(ifs);
    end = std::chrono::steady_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    GTEST_LOG_(INFO) << "Deserialization took: " << duration.count() << " ms";
    ifs.close();
}
TEST(SerializeTest, Binary)
{
    // CBOR byte string with payload 0xCAFE
    std::vector<std::uint8_t> v = {0x42, 0xCA, 0xFE};
    json j = json::from_cbor(v);
    auto &binary = j.get_binary();
}