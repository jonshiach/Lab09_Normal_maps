#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/io.hpp>

#include "shader.hpp"
#include "camera.hpp"
#include "model.hpp"
#include "light.hpp"

// Create camera object
Camera camera(glm::vec3(0.0f, 1.0f, 5.0f));

// Timers
float currentTime = 0.0f;
float lastTime = 0.0f;
float deltaTime = 0.0f;

int main( void )
{
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    GLFWwindow* window = glfwCreateWindow( 1024, 768, "Normal Mapping", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible.");
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    // Enable depth test
    glEnable(GL_DEPTH_TEST);

    // Use back face culling
    glEnable(GL_CULL_FACE);

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwPollEvents();
    glfwSetCursorPos(window, 1024/2, 768/2);

    // Compile shader programs
    GLuint shaderID = LoadShaders("vertexShader.vert", "fragmentShader.frag");
    
    // Load models
    Model teapot("../objects/teapot.obj");
    Model lightModel("../objects/sphere.obj");
   
    // Add textures to models
    teapot.addTexture("../objects/blue_diffuse.bmp", "diffuse");

    // Specify world space object positions
    glm::vec3 positions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -10.0f),
        glm::vec3(-3.0f, -2.0f, -4.0f),
        glm::vec3(-4.0f, -2.0f, -8.0f),
        glm::vec3( 2.0f, -1.0f, -4.0f),
        glm::vec3(-4.0f,  3.0f, -10.0f),
        glm::vec3( 0.0f, -2.0f, -8.0f),
        glm::vec3( 4.0f,  2.0f, -6.0f),
        glm::vec3( 3.0f,  0.0f, -1.0f),
        glm::vec3(-1.0f,  1.0f, -2.0f)
    };
    
    // Define light colours
    glm::vec3 white = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 yellow = glm::vec3(1.0f, 1.0f, 0.0f);
    glm::vec3 black = glm::vec3(0.0f, 0.0f, 0.0f);
    
    // Specify light sources
    Light lightSources;
    lightSources.addPointLight(glm::vec3(2.0f, 2.0f, 2.0f), white);
    lightSources.addSpotLight(glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), white, cos(glm::radians(45.0f)));
    lightSources.addDirLight(glm::vec3(1.0f, 0.0f, 0.0f), yellow);
    
    // Compile light shader program
    lightSources.lightShaderID = LoadShaders("lightVertexShader.vert", "lightFragmentShader.frag");
    
    do {
        // Update timers
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // Background colour
        glm::vec3 bgColour = glm::vec3(0.0f, 0.0f, 0.0f);
        glClearColor(bgColour[0], bgColour[1], bgColour[2], 1.0f);
        
        // Clear the window
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Get the view and projection matrices from the camera library
        camera.calculateMatrices(window, deltaTime);
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = camera.getProjectionMatrix();

        // Activate shader
        glUseProgram(shaderID);
        
        // Send camera position, view and projection matrices to the shader
        glUniformMatrix4fv(glGetUniformLocation(shaderID, "view"), 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, &projection[0][0]);

        // Send light source properties to the shader
        lightSources.toShader(shaderID);
    
        // Loop through objects
        for (unsigned int i = 0; i < 10; i++)
        {
            // Calculate model matrix
            glm::mat4 translate = glm::translate(glm::mat4(1.0f), positions[i]);
            glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
            glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), 30.0f * i, glm::vec3(1.0f));
            glm::mat4 model = translate * rotate * scale;
            
            // Send the model matrix to the shader
            glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, &model[0][0]);
            
            // Send material properties to the shader
            glUniform1f(glGetUniformLocation(shaderID, "ka"), teapot.ka);
            glUniform1f(glGetUniformLocation(shaderID, "kd"), teapot.kd);
            glUniform1f(glGetUniformLocation(shaderID, "ks"), teapot.ks);
            glUniform1f(glGetUniformLocation(shaderID, "Ns"), teapot.Ns);
            
            // Draw the model
            teapot.draw(shaderID);
        }
        
        // Draw the light sources
        lightSources.draw(view, projection, lightModel);
        
        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}
