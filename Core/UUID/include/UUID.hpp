#pragma once
#include <array>
#include <cstdint>
#include <string>
namespace MEngine::Core
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
    operator bool() const
    {
        return *this == UUID{};
    }
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
} // namespace MEngine::Core

namespace std
{
template <> struct hash<MEngine::Core::UUID>
{
    std::size_t operator()(const MEngine::Core::UUID &uuid) const noexcept
    {
        const auto &data = uuid.GetData();
        std::size_t hash = 14695981039346656037ULL; // FNV-1a 64-bit prime

        // Process 16 bytes in chunks for better performance
        for (size_t i = 0; i < 16; i += 4)
        {
            // Combine 4 bytes at a time into a 32-bit chunk
            uint32_t chunk = (static_cast<uint32_t>(data[i]) << 24) | (static_cast<uint32_t>(data[i + 1]) << 16) |
                             (static_cast<uint32_t>(data[i + 2]) << 8) | static_cast<uint32_t>(data[i + 3]);
            hash ^= chunk;
            hash *= 1099511628211ULL; // FNV-1a 64-bit multiplier
        }

        return hash;
    }
};
} // namespace std