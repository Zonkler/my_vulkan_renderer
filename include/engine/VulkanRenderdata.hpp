#pragma once
#include <SDL.h>
#include <SDL_vulkan.h>
#include <vector>

struct VulkanRenderData {
    SDL_Window* window = nullptr;
    
    const char * appname = nullptr;
    
    int rdWidth = 0;
    
    int rdHeight = 0;
  
    std::vector<const char*> extensions;

    std::vector<const char*> layers;
  
  //unsigned int rdTriangleCount = 0;
  //unsigned int rdMatricesSize = 0;

  //std::vector<Light> Lights;
  //int rdLightIndex=0;
  //const int rdMaxLights=32;

  //int rdFieldOfView = 60;

  //float rdFrameTime = 0.0f;
  //float rdMatrixGenerateTime = 0.0f;
  //float rdUploadToVBOTime = 0.0f;
  //float rdUploadToUBOTime = 0.0f;
  //float rdUIGenerateTime = 0.0f;
  //float rdUIDrawTime = 0.0f;

  //int rdMoveForward = 0;
  //int rdMoveRight = 0;
  //int rdMoveUp = 0;

  //float rdViewAzimuth = 330.0f;
  //float rdViewElevation = -20.0f;
  //glm::vec3 rdCameraWorldPosition = glm::vec3(2.0f, 5.0f, 7.0f);
};