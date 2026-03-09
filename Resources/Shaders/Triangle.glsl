#shader vertex

// This is the version we are going to use for this shader
#version 330 core

// Inputs
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 a_color;

out vec3 node_color;

void main()
{
    gl_Position = vec4(aPos.xyz, 1.0);
    node_color = a_color;
}

#shader fragment
#version 330 core

// inputs
in vec3 node_color;

// Outputs
out vec4 FragColor;

// Clarify what are uniforms? -> Global read only variables.
void main()
{
    FragColor = vec4(node_color, 1.0);
}
