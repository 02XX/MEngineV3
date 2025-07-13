#include <cstring>
#include <gtest/gtest.h>
#include <print>
#include <ranges>
#include <string>
#include <vector>

TEST(StringTest, vector)
{
    std::string str = "Hello, World!";
    EXPECT_EQ(str.size(), 13);
    std::vector<char> strVec(std::from_range, str);
    strVec.push_back('\0'); 
    EXPECT_EQ(strVec.size(), 14);
    std::print("Vec Capacity: {}\n", strVec.capacity());
    char *strData = strVec.data();
    EXPECT_EQ(strlen(strData), 13);
    std::print("String data: {}\n", strData);
}