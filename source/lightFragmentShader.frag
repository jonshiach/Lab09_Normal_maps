#version 330 core

// Output data
out vec3 colour;

// Uniforms
uniform vec3 lightColour;

void main ()
{
    colour = lightColour;
}
