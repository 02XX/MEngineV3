#pragma once
#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>
namespace MEngine
{
class Vertex
{
  public:
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
    static std::array<vk::VertexInputAttributeDescription, 3> GetVertexInputAttributeDescription();
    static vk::VertexInputBindingDescription GetVertexInputBindingDescription();
};
} // namespace MEngine