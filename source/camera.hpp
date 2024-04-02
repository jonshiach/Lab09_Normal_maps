#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Camera
{
    
public:
    // Projection parameters
    float fov = glm::radians(45.0f);
    float aspect = 1024.0f / 768.0f;
    float near = 0.02f;
    float far = 100.0f;
    
    // Camera vectors
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 worldUp;
    glm::vec3 front;
    glm::vec3 right;
    glm::vec3 up;
    
    // Transformation matrices
    glm::mat4 view;
    glm::mat4 projection;
    
    // Speed attributes
    float speed = 5.0f;
    float mouseSpeed = 0.005f;
    
    // Camera angles
    const float pi = 3.1415927f;
    float pitch;
    float yaw;
    
    // Constructor
    Camera(const glm::vec3 Position);
    
    // Methods
    glm::mat4 getViewMatrix();
    glm::mat4 getProjectionMatrix();
    void calculateMatrices(GLFWwindow* window, const float deltaTime);
};
