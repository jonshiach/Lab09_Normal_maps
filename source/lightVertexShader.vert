#version 330 core

// Input vertex data
layout(location = 0) in vec3 position;

struct Light
{
    vec3 position;
    vec3 colour;
};

// Uniforms
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform Light light;

void main()
{
    // Output vertex postion
    gl_Position = projection * view * model * vec4(position, 1.0);
}
