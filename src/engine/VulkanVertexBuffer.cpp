#include "engine/VulkanVertexBuffer.hpp"
#include <iostream>

VertexInputDescription Vertex::get_vertex_description(){

    VertexInputDescription description;

    VkVertexInputBindingDescription mainBinding{
        .binding = 0,
        .stride = sizeof(Vertex),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX

    };

    description.bindings.push_back(mainBinding);


    //Position will be stored at Location 0
	VkVertexInputAttributeDescription positionAttribute = {};
	positionAttribute.binding = 0;
	positionAttribute.location = 0;
	positionAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
	positionAttribute.offset = offsetof(Vertex, position);


    	//Color will be stored at Location 2
	VkVertexInputAttributeDescription colorAttribute = {};
	colorAttribute.binding = 0;
	colorAttribute.location = 1;
	colorAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
	colorAttribute.offset = offsetof(Vertex, color);

	description.attributes.push_back(positionAttribute);
	description.attributes.push_back(colorAttribute);
    std::cout<<"ATTRIBUTES CREATED\n";
	return description;

}



Model::Model(VmaAllocator& allocator) : m_allocator(allocator)
{
    load_Model();
    upload_Model();
}

Model::~Model()
{
    vmaDestroyBuffer(m_allocator,m_mesh.m_vertexBuffer.m_buffer,m_mesh.m_vertexBuffer.m_allocation);
    std::cout<<"DESTROYED VMA ALLOCATION OF BUFFER\n";

}




void Model::load_Model(){

    m_mesh.m_vertices.resize(3);
    

    m_mesh.m_vertices[0].position = {0.0f, -0.5f};
    m_mesh.m_vertices[1].position = {0.5f, 0.5f};
    m_mesh.m_vertices[2].position = {-0.4f, 0.4f};

    m_mesh.m_vertices[0].color = {1.0, 0.0, 0.0};
    m_mesh.m_vertices[1].color = {0.0, 1.0, 0.0};
    m_mesh.m_vertices[2].color = {0.0, 0.0, 1.0};

    
}

void Model::upload_Model(){

    uint32_t size = m_mesh.m_vertices.size() * sizeof(m_mesh.m_vertices[0]);

    VkBufferCreateInfo bufferInfo{

        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = size,
        .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
    };

    VmaAllocationCreateInfo vmaallocinfo{
        .usage = VMA_MEMORY_USAGE_GPU_TO_CPU
    };

    vmaCreateBuffer(m_allocator,&bufferInfo,&vmaallocinfo,&m_mesh.m_vertexBuffer.m_buffer,&m_mesh.m_vertexBuffer.m_allocation,nullptr);

    void* data;
    vmaMapMemory(m_allocator,m_mesh.m_vertexBuffer.m_allocation,&data);
    memcpy(data,m_mesh.m_vertices.data(),size);
    vmaUnmapMemory(m_allocator,m_mesh.m_vertexBuffer.m_allocation);
   std::cout<<"MEMORY AND BUFFER CREATED with SIZE"<<size<<"\n";

}