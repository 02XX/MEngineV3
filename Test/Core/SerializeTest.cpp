#include "gtest/gtest.h"
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
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
} // namespace nlohmann

TEST(Entity, serialize)
{
    auto entity = std::make_shared<ABC>();
    nlohmann::json j;
    EXPECT_NO_THROW(j = *entity);
    GTEST_LOG_(INFO) << j.dump(4);
}
TEST(Entity, deserialize)
{
    nlohmann::json j;
    j["name"] = "test";
    auto entity = std::make_shared<ABC>();
    EXPECT_NO_THROW(j.get_to(*entity));
    GTEST_LOG_(INFO) << entity->age;
    EXPECT_EQ(entity->age, 2);
}