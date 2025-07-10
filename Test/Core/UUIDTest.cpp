#include "UUID.hpp"
#include "UUIDGenerator.hpp"
#include "gtest/gtest.h"
#include <gtest/gtest.h>

using namespace MEngine::Core;
TEST(UUIDTest, GenerateUUID)
{
    auto uuidGenerator = UUIDGenerator();
    auto uuid = uuidGenerator.Create();
    EXPECT_EQ(uuid.GetData().size(), 16);
    EXPECT_NE(uuid.ToString().empty(), true);
}
TEST(UUIDTest, UUIDEquality)
{
    auto uuidGenerator = UUIDGenerator();
    auto uuid1 = uuidGenerator.Create();
    auto uuid2 = uuidGenerator.Create();
    EXPECT_NE(uuid1, uuid2);

    auto uuid3 = UUID(uuid1.ToString());
    EXPECT_EQ(uuid1, uuid3);
}
TEST(UUIDTest, UUIDFromString)
{
    auto uuidGenerator = UUIDGenerator();
    auto uuid1 = uuidGenerator.Create();
    auto uuid2 = UUID(uuid1.ToString());
    EXPECT_EQ(uuid1, uuid2);
}
TEST(UUIDTest, UUIDToString)
{
    auto uuidGenerator = UUIDGenerator();
    auto uuid = uuidGenerator.Create();
    std::string uuidStr = uuid.ToString();
    EXPECT_EQ(uuidStr.size(), 36);                   // UUID string format is 8-4-4-4-12
    EXPECT_NE(uuidStr.find('-'), std::string::npos); // Should contain dashes
}
TEST(UUIDTest, UUIDHash)
{
    auto uuidGenerator = UUIDGenerator();
    auto uuid1 = uuidGenerator.Create();
    auto uuid2 = uuidGenerator.Create();
    std::hash<UUID> hashFunction;
    size_t hash1 = hashFunction(uuid1);
    size_t hash2 = hashFunction(uuid2);
    EXPECT_NE(hash1, hash2); // Different UUIDs should have different hashes
}
TEST(UUIDTest, constructor)
{
    auto uuidGenerator = UUIDGenerator();
    UUID uuid1 = uuidGenerator.Create();
    UUID uuid2 = uuid1; // Copy constructor
    EXPECT_EQ(uuid1, uuid2);
}
TEST(UUIDTest, custom)
{
    std::string uuidStr = "00000000-0000-0000-0000-000000000001";
    auto uuid = UUID{uuidStr};
    EXPECT_EQ(uuidStr, uuid.ToString());
    GTEST_LOG_(INFO) << uuid.ToString();
    GTEST_LOG_(INFO) << UUID{"00000000-0000-0000-0000-000000000002"}.ToString();
}
