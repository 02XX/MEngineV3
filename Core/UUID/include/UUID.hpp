#pragma once
#include <array>
#include <cstdint>
#include <string>
namespace MEngine
{
/**
 * @brief UUIDv7 https://www.rfc-editor.org/rfc/rfc9562.html#name-uuid-version-7
 *
 */
class UUID
{
  private:
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
    std::array<uint8_t, 16> mData;

  public:
    UUID() = default;
    inline explicit UUID(const std::array<uint8_t, 16> &data) : mData(data)
    {
    }
    explicit UUID(const char *str);
    explicit UUID(const std::string &str);
    std::string ToString() const;
    inline const std::array<uint8_t, 16> &GetData() const
    {
        return mData;
    }
    inline bool operator==(const UUID &other) const
    {
        return mData == other.mData;
    }
    inline bool operator!=(const UUID &other) const
    {
        return !(*this == other);
    }
};
} // namespace MEngine