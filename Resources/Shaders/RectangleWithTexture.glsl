#shader vertex

// This is the version we are going to use for this shader
#version 330 core

// Inputs
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;

uniform mat4 transformation_vector;

void main()
{
//    gl_Position = vec4(aPos, 1.0);
    ourColor = aColor;
//    TexCoord = aTexCoord;
    gl_Position = transformation_vector * vec4(aPos, 1.0f);
    TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}

#shader fragment
#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D texture0;
uniform sampler2D texture1;

void main()
{
//    FragColor = texture(texture1, TexCoord);
    // The final output color is now the combination of two texture lookups.
    // GLSL's built-in mix function takes two values as input and linearly interpolates between them based on its third argument.
    // If the third value is 0.0 it returns the first input; if it's 1.0 it returns the second input value.
    // A value of 0.2 will return 80% of the first input color and 20% of the second input color, resulting in a mixture of both our textures.
    FragColor = mix(texture(texture0, TexCoord), texture(texture1, TexCoord), 0.5);
}
