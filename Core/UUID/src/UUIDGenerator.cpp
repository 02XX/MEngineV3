#include "UUIDGenerator.hpp"

namespace MEngine::Core
{
UUIDGenerator::UUIDGenerator()
{
    mUUIDGenerator = std::make_unique<UUIDv7Generator>();
}
UUIDGenerator &UUIDGenerator::Instance()
{
    static UUIDGenerator instance;
    return instance;
}
UUID UUIDGenerator::Create()
{
    return mUUIDGenerator->Create();
}
} // namespace MEngine::Core