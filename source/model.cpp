#include <vector>
#include <string>
#include <cstring>
#include <iostream>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtx/io.hpp>

#include "model.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.hpp"

// Constructor
Model::Model(const char *path)
{
    // Load object
    loadObj(path);
    
    // Add neutral textures
    addTexture("../objects/neutral_normal.png", "normal");
    addTexture("../objects/neutral_specular.png", "specular");

    // Setup buffers
    setupBuffers();
}

// Draw the model
void Model::draw(GLuint &shaderID)
{
    // Bind the textures
    for (unsigned int i = 0; i < textures.size(); i++)
    {
        // Bind texture
        glActiveTexture(GL_TEXTURE0 + i);
        glUniform1i(glGetUniformLocation(shaderID, (textures[i].type + "Map").c_str()), i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }
    
    // Draw the triangles
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<unsigned int>(vertices.size()));
    glBindVertexArray(0);
    
    // Reset active texture
    glActiveTexture(GL_TEXTURE0);
    
    // Disable the vertex arrays
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(4);
}

// Setup and copy data to the buffers
void Model::setupBuffers()
{    
    // Create and bind the Vertex Array Object (VAO)
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    // Create Vertex Buffer Object
    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    
    // Create uv buffer
    GLuint uvBuffer;
    glGenBuffers(1, &uvBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
    
    // Create normal buffer
    GLuint normalBuffer;
    glGenBuffers(1, &normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
   
    // Bind the vertex buffer
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    // Bind the uv buffer
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    // Bind the normal buffer
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
     // Unbind the VAO
    glBindVertexArray(0);
}

// Load an .obj model
void Model::loadObj(const char *path)
{
    
    printf("Loading file %s\n", path);
    
    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
    std::vector<glm::vec3> tempVertices;
    std::vector<glm::vec2> tempUVs;
    std::vector<glm::vec3> tempNormals;
    
    FILE *file = fopen(path, "r");
    if (file==NULL)
    {
        printf("Impossible to open the file. Check paths and directories.");
        getchar();
        return false;
    }
    
    while (true)
    {
        // Read the first word of the line
        char lineHeader[128];
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
        {
            // If end of file reached exit the loop
            break;
        }
        
        if (strcmp(lineHeader, "v") == 0)
        {
            // Read vertices
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            tempVertices.push_back(vertex);
        }
        else if (strcmp(lineHeader, "vt") == 0)
        {
            // Read texture co-ordinates
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y);
            tempUVs.push_back(uv);
        }
        else if (strcmp(lineHeader, "vn") == 0)
        {
            // Read vertex normals
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            tempNormals.push_back(normal);
        }
        else if (strcmp(lineHeader, "f") == 0)
        {
            // Read vertex indices
            std::string vertex1, vertex2, vertex3;
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
                                 &vertexIndex[0], &uvIndex[0], &normalIndex[0],
                                 &vertexIndex[1], &uvIndex[1], &normalIndex[1],
                                 &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
            
            // Check for error
            if (matches != 9)
            {
                printf("File can't be read by loadObj().\n");
                fclose(file);
                return false;
            }
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
            uvIndices    .push_back(uvIndex[0]);
            uvIndices    .push_back(uvIndex[1]);
            uvIndices    .push_back(uvIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);
        }
        else
        {
            // Remove comment line
            char commentBuffer[1000];
            fgets(commentBuffer, 1000, file);
        }
    }
    
    // For each vertex of the triangle
    for (unsigned int i = 0; i < vertexIndices.size(); i++)
    {
        // Get the indices of its attributes
        unsigned int vertexIndex = vertexIndices[i];
        unsigned int uvIndex = uvIndices[i];
        unsigned int normalIndex = normalIndices[i];
        
        // Get the attributes
        glm::vec3 vertex = tempVertices[vertexIndex - 1];
        glm::vec2 uv = tempUVs[uvIndex - 1];
        glm::vec3 normal = tempNormals[normalIndex - 1];
        
        // Copy the attributes to the buffers
        vertices.push_back(vertex);
        uvs.push_back(uv);
        normals.push_back(normal);
    }
    
    // Close .obj file
    fclose(file);
}

// Add textures to the model
void Model::addTexture(const char *path, const std::string type)
{
    Texture texture;
    texture.id = loadTexture(path);
    texture.type = type;
    textures.push_back(texture);
}

// Load texture from file (using stb_image)
unsigned int Model::loadTexture(const char *path)
{

    unsigned int textureID;
    glGenTextures(1, &textureID);
    int width, height, numComponents;
    stbi_set_flip_vertically_on_load(true);  
    unsigned char *data = stbi_load(path, &width, &height, &numComponents, 0);
    
    if (data)
    {
        GLenum format;
        if (numComponents == 1)
            format = GL_RED;
        else if (numComponents == 3)
            format = GL_RGB;
        else if (numComponents == 4)
            format = GL_RGBA;
        
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture " << path << " failed to load." << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
