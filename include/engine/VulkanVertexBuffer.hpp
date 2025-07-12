#pragma once
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include <glm/glm.hpp>
#include <vector>


struct VertexInputDescription {

	std::vector<VkVertexInputBindingDescription> bindings;
	std::vector<VkVertexInputAttributeDescription> attributes;

	VkPipelineVertexInputStateCreateFlags flags = 0;
};


struct Vertex
{
    glm::vec3 position;
    glm::vec3 color;

    static VertexInputDescription get_vertex_description();

};

struct AllocatedBuffer {
    VkBuffer m_buffer;
    VmaAllocation m_allocation;
};


struct Mesh {
	std::vector<Vertex> m_vertices;

	AllocatedBuffer m_vertexBuffer;
};





class Model
{
public:
    Mesh m_mesh;
    VertexInputDescription m_inputDescription;
    VmaAllocator m_allocator;
public:
    Model(VmaAllocator& allocator);
    ~Model();
    void load_Model();
    void upload_Model();
};


