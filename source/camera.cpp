#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.hpp"

Camera::Camera(const glm::vec3 Position)
{
    position = Position;
    target = glm::vec3(0.0f, 0.0f, 0.0f);
    worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    front = target - position;
    yaw = pi + tan(front[0] / front[2]);
    pitch = pi + tan(front[1] / front[2]);
}

glm::mat4 Camera::getViewMatrix()
{
    return view;
}

glm::mat4 Camera::getProjectionMatrix()
{
    return projection;
}

void Camera::calculateMatrices(GLFWwindow *window, const float deltaTime)
{
    // Keyboard inputs
    if (glfwGetKey(window, GLFW_KEY_W))
        position += front * deltaTime * speed;

    if (glfwGetKey(window, GLFW_KEY_S))
        position -= front * deltaTime * speed;

    if (glfwGetKey(window, GLFW_KEY_A))
        position -= right * deltaTime * speed;

    if (glfwGetKey(window, GLFW_KEY_D))
        position += right * deltaTime * speed;
    
    // Get mouse cursor position
    double xPos, yPos;
    glfwGetCursorPos(window, &xPos, &yPos);
    glfwSetCursorPos(window, 1024/2, 768/2);
    
    // Update yaw and pitch angles
    yaw += mouseSpeed * float(xPos - 1024/2);
    pitch += mouseSpeed * float(yPos - 768/2);
    
    // Update camera vectors
    front = glm::normalize(glm::vec3(sin(yaw) * cos(pitch), sin(pitch), -cos(yaw) * cos(pitch)));
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
    target = position + front;
    
    // Calculate view matrix
    view = glm::lookAt(position, position + front, up);
    
    // Calculate projection matrix
    projection = glm::perspective(fov, aspect, near, far);
}
