#include "UUID.hpp"

namespace MEngine
{
class IUUIDGenerator
{
  public:
    virtual ~IUUIDGenerator() = default;
    virtual UUID Create() = 0;
};
} // namespace MEngine