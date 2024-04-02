#include <string>
#include <iostream>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/io.hpp>

#include "model.hpp"
#include "light.hpp"

void Light::addPointLight(const glm::vec3 Position, const glm::vec3 Colour)
{
    LightSource light;
    light.position = Position;
    light.colour = Colour;
    light.type = "point";
    lights.push_back(light);
    numPoint++;
}

void Light::addSpotLight(const glm::vec3 Position, const glm::vec3 Direction, const glm::vec3 Colour, const float CosPhi)
{
    LightSource light;
    light.position = Position;
    light.direction = Direction;
    light.colour = Colour;
    light.cosPhi = CosPhi;
    light.type = "spot";
    lights.push_back(light);
    numSpot++;
}

void Light::addDirLight(const glm::vec3 Direction, const glm::vec3 Colour)
{
    LightSource light;
    light.direction = Direction;
    light.colour = Colour;
    light.type = "directional";
    lights.push_back(light);
    numDir++;
}

void Light::toShader(GLuint shaderID)
{
    reorder();
    glUniform1i(glGetUniformLocation(shaderID, "numPoint"), numPoint);
    glUniform1i(glGetUniformLocation(shaderID, "numSpot"), numSpot);
    glUniform1i(glGetUniformLocation(shaderID, "numDir"), numDir);
    
    for (unsigned int i = 0; i < lights.size(); i++)
    {
        std::string number = std::to_string(i);
        glUniform3fv(glGetUniformLocation(shaderID, ("lights[" + number + "].position").c_str()), 1, &lights[i].position[0]);
        glUniform3fv(glGetUniformLocation(shaderID, ("lights[" + number + "].direction").c_str()), 1, &lights[i].direction[0]);
        glUniform3fv(glGetUniformLocation(shaderID, ("lights[" + number + "].colour").c_str()), 1, &lights[i].colour[0]);
        glUniform1f(glGetUniformLocation(shaderID, ("lights[" + number + "].constant").c_str()), lights[i].constant);
        glUniform1f(glGetUniformLocation(shaderID, ("lights[" + number + "].linear").c_str()), lights[i].linear);
        glUniform1f(glGetUniformLocation(shaderID, ("lights[" + number + "].quadratic").c_str()), lights[i].quadratic);
        glUniform1f(glGetUniformLocation(shaderID, ("lights[" + number + "].cosPhi").c_str()), lights[i].cosPhi);
    }
}

void Light::draw(glm::mat4 view, glm::mat4 projection, Model lightModel)
{
    glUseProgram(lightShaderID);
    for (unsigned int i = 0; i < lights.size(); i++)
    {
        if (lights[i].type == "directional")
            continue;
        
        // Calculate model matrix
        glm::mat4 translate = glm::translate(glm::mat4(1.0f), lights[i].position);
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f));
        glm::mat4 model = translate * scale;
        
        // Send model, view, projection matrices and light colour to light shader
        glUniformMatrix4fv(glGetUniformLocation(lightShaderID, "model"), 1, GL_FALSE, &model[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(lightShaderID, "view"), 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(lightShaderID, "projection"), 1, GL_FALSE, &projection[0][0]);
        glUniform3fv(glGetUniformLocation(lightShaderID, "lightColour"), 1, &lights[i].colour[0]);
        
        // Draw light source
        lightModel.draw(lightShaderID);
    }
}

void Light::reorder()
{
    std::vector<LightSource> temp;
    for (unsigned int i = 0; i < lights.size(); i++)
    {
        if (lights[i].type == "point")
            temp.push_back(lights[i]);
    }
    for (unsigned int i = 0; i < lights.size(); i++)
    {
        if (lights[i].type == "spot")
            temp.push_back(lights[i]);
    }
    for (unsigned int i = 0; i < lights.size(); i++)
    {
        if (lights[i].type == "directional")
            temp.push_back(lights[i]);
    }
    lights = temp;
}
