#pragma once

#include "engine/VulkanVertexBuffer.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

struct transformComponent
{
    glm::vec3 translation{};
    glm::vec3 scale{1.f,1.f,1.f};
    glm::vec3 rotation{};
    
    glm::mat4 mat4(){
       auto transform = glm::translate(glm::mat4{1.f},translation); 
       transform = glm::rotate(transform,rotation.y,{0.f,1.f,0.f});
       transform = glm::rotate(transform,rotation.x,{1.f,0.f,0.f});
       transform = glm::rotate(transform,rotation.z,{0.f,0.f,1.f});

       transform = glm::scale(transform,scale);
       return transform;
    }
};


class gameobject
{
public:
    using id_t = unsigned int;

    static gameobject createGameObject(){
        static id_t currentId=0;
        return gameobject(currentId++);
    }

    std::shared_ptr<Model> model{};
    glm::vec3 color{};
    transformComponent transform2D;

    id_t getId() const {return id;}
    ~gameobject() = default;
private:
    id_t id;
    gameobject(id_t objId) : id(objId){};



};

//gameobject::gameobject(/* args */)
//{
//}

//gameobject::~gameobject()
//{
//}
