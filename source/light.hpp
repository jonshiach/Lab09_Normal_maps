#pragma once

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "model.hpp"

struct LightSource
{
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 direction = glm::vec3(1.0f);
    glm::vec3 colour = glm::vec3(1.0f);
    float constant = 1.0;
    float linear = 0.1;
    float quadratic = 0.02;
    float cosPhi;
    std::string type;
};

class Light
{
public:
    std::vector<LightSource> lights;
    float ka = 0.2f;
    float kd = 0.7f;
    float ks = 1.0f;
    unsigned int numPoint = 0;
    unsigned int numSpot = 0;
    unsigned int numDir = 0;
    GLuint lightShaderID;
    
    // Add lights
    void addPointLight(const glm::vec3 Position, const glm::vec3 Colour);
    void addSpotLight(const glm::vec3 Position, const glm::vec3 Direction, const glm::vec3 Colour, const float CosPhi);
    void addDirLight(const glm::vec3 Direction, const glm::vec3 Colour);
    
    // Send to shader
    void toShader(GLuint shaderID);
    
    // Draw light source
    void draw(glm::mat4 view, glm::mat4 projection, Model lightModel);
    
private:
    void reorder();
};
