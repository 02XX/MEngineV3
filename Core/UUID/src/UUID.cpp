#include "UUID.hpp"
#include <cstddef>
#include <stdexcept>

namespace MEngine::Core
{
UUID::UUID(const char *str) : UUID(std::string(str))
{
}
UUID::UUID(const std::string &str)
{
    if (str.size() != 36)
    {
        throw std::invalid_argument("Invalid UUID string length");
    }
    mData.fill(0);
    size_t j = 0;
    for (size_t i = 0; i < str.size(); ++i)
    {
        if (str[i] == '-')
        {
            continue;
        }
        if (j >= 32)
        {
            throw std::out_of_range("UUID string too long");
        }
        char c = str[i];
        uint8_t val = 0;
        if (c >= '0' && c <= '9')
        {
            val = c - '0';
        }
        else if (c >= 'a' && c <= 'f')
        {
            val = c - 'a' + 10;
        }
        else if (c >= 'A' && c <= 'F')
        {
            val = c - 'A' + 10;
        }
        else
        {
            throw std::invalid_argument("Invalid character in UUID string");
        }
        if (j % 2 == 0)
        {
            mData[j / 2] = val << 4;
        }
        else
        {
            mData[j / 2] |= val;
        }
        ++j;
    }
    if (j != 32)
    {
        throw std::invalid_argument("UUID string too short");
    }
}

std::string UUID::ToString() const
{
    // -------------------------------------------
    // field       bits value
    // -------------------------------------------
    // unix_ts_ms  48   0x017F22E279B0
    // ver          4   0x7
    // rand_a      12   0xCC3
    // var          2   0b10
    // rand_b      62   0b01, 0x8C4DC0C0C07398F
    // -------------------------------------------
    // total       128
    // -------------------------------------------
    // final: 017F22E2-79B0-7CC3-98C4-DC0C0C07398F
    static const char *hex = "0123456789abcdef";
    std::string result(36, ' ');

    int pos = 0;
    for (int i = 0; i < 16; ++i)
    {
        // 每4个字节后插入'-'，插入位置是8, 13, 18, 23
        if (pos == 8 || pos == 13 || pos == 18 || pos == 23)
        {
            result[pos++] = '-';
        }

        result[pos++] = hex[(mData[i] >> 4) & 0xF];
        result[pos++] = hex[mData[i] & 0xF];
    }
    return result;
}
} // namespace MEngine::Core