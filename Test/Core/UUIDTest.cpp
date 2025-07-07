#include "UUID.hpp"
#include "UUIDGenerator.hpp"
#include "gtest/gtest.h"
#include <gtest/gtest.h>

using namespace MEngine;

TEST(UUIDTest, GenerateUUID)
{
    UUIDGenerator uuidGen;
    UUID uuid1 = uuidGen.Create();
    UUID uuid2 = uuidGen.Create();
    EXPECT_NE(uuid1, uuid2);
}
TEST(UUIDTest, GenerateZeroUUID)
{
    UUID uuid = UUID();
    EXPECT_EQ(uuid.ToString(), "00000000-0000-0000-0000-000000000000");
}
TEST(UUIDTest, UUIDToString)
{
    UUIDGenerator uuidGen;
    UUID uuid = uuidGen.Create();
    std::string uuidStr = uuid.ToString();
    EXPECT_EQ(uuidStr.size(), 36); // UUID string representation is 36 characters long
}

TEST(UUIDTest, UUIDFromString)
{
    UUIDGenerator uuidGen;
    UUID uuid = uuidGen.Create();
    std::string uuidStr = uuid.ToString();
    UUID parsedUUID = UUID(uuidStr);
    EXPECT_EQ(uuid, parsedUUID);
    EXPECT_EQ(uuid.ToString(), parsedUUID.ToString());
}
TEST(UUIDTest, UUIDFormat)
{
    UUIDGenerator uuidGen;
    UUID uuid1 = uuidGen.Create();
}

TEST(UUIDTest, UUIDEquality)
{
    UUIDGenerator uuidGen;
    UUID uuid1 = uuidGen.Create();
    UUID uuid2 = uuidGen.Create();
    EXPECT_NE(uuid1, uuid2);

    UUID uuid3 = uuid1; // Copy constructor
    EXPECT_EQ(uuid1, uuid3);

    UUID uuid4 = UUID(uuid1.ToString()); // From string
    EXPECT_EQ(uuid1, uuid4);
}

TEST(UUIDTest, 10UUID)
{
    for (int i = 0; i < 10; ++i)
    {
        UUIDGenerator uuidGen;
        UUID uuid = uuidGen.Create();
        GTEST_LOG_(INFO) << "Generated UUID: " << uuid.ToString();
    }
}