#include "Vertex.hpp"

namespace MEngine::Core
{
std::vector<vk::VertexInputAttributeDescription> Vertex::GetVertexInputAttributeDescription()
{
    std::vector<vk::VertexInputAttributeDescription> attributeDescriptions(3);
    attributeDescriptions[0].setBinding(0).setLocation(0).setFormat(vk::Format::eR32G32B32Sfloat).setOffset(0);
    attributeDescriptions[1]
        .setBinding(0)
        .setLocation(1)
        .setFormat(vk::Format::eR32G32B32Sfloat)
        .setOffset(offsetof(Vertex, normal));
    attributeDescriptions[2]
        .setBinding(0)
        .setLocation(2)
        .setFormat(vk::Format::eR32G32Sfloat)
        .setOffset(offsetof(Vertex, texCoords));
    return attributeDescriptions;
}
vk::VertexInputBindingDescription Vertex::GetVertexInputBindingDescription()
{
    vk::VertexInputBindingDescription bindingDescription;
    bindingDescription.setBinding(0).setStride(sizeof(Vertex)).setInputRate(vk::VertexInputRate::eVertex);
    return bindingDescription;
}
} // namespace MEngine::Core