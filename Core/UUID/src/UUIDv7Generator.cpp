#include "UUIDv7Generator.hpp"
#include <chrono>
#include <cstddef>
#include <random>
namespace MEngine::Core
{
// https://datatracker.ietf.org/doc/html/rfc9562#name-example-of-a-uuidv7-value
UUID UUIDv7Generator::Create()
{
    std::array<uint8_t, 16> data = {};
    // unix_ts_ms 48-bit big-endian unsigned number of the Unix Epoch timestamp in milliseconds
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    uint64_t unix_ts_ms = static_cast<uint64_t>(ms.count());
    unix_ts_ms &= 0x0000FFFFFFFFFFFF; // Mask to 48 bits
    for (size_t i = 0; i < 6; ++i)    // big-endian
    {
        data[i] = static_cast<uint8_t>((unix_ts_ms >> (8 * (5 - i))) & 0xFF);
    }
    // ver The 4-bit version field as defined by Section 4.2, set to 0b0111 (7). Occupies bits 48 through 51 of octet 6.
    data[6] = (data[6] & 0x0F) | 0x70; // Set version to 7
    // rand_a 12 bits of pseudorandom data to provide uniqueness as per Section 6.9 and/or optional constructs to
    // guarantee additional monotonicity as per Section 6.2. Occupies bits 52 through 63 (octets 6-7).
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dist(0, 0xFFF); // 12 bits
    uint64_t rand_a = dist(gen);
    data[6] = (data[6] & 0xF0) | static_cast<uint8_t>((rand_a >> 8) & 0x0F); // Set rand_a in bits 52-63
    data[7] = static_cast<uint8_t>(rand_a & 0xFF);                           // Set rand_a in bits 64-71
    // var The 2-bit variant field as defined by Section 4.1, set to 0b10. Occupies bits 64 and 65 of octet 8.
    data[8] = (data[8] & 0x3F) | 0x80; // Set variant to 10xx
    // rand_b The final 62 bits of pseudorandom data to provide uniqueness as per Section 6.9 and/or an optional counter
    // to guarantee additional monotonicity as per Section 6.2. Occupies bits 66 through 127 (octets 8-15).
    std::uniform_int_distribution<uint64_t> dist2(0, 0x3FFFFFFFFFFFFFFF); // 62 bits
    uint64_t rand_b = dist2(gen);
    for (size_t i = 9; i < 16; ++i)
    {
        data[i] = static_cast<uint8_t>((rand_b >> (8 * (15 - i))) & 0xFF);
    }
    // Create UUID from data
    return UUID(data);
}
} // namespace MEngine::Core