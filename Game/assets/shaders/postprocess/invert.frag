#version 460 core

out vec4 FragColor;

in vec2 TexCoords;

layout(binding = 0) uniform sampler2D screenTexture;

void main()
{
    vec3 color = texture(screenTexture, TexCoords).rgb;
    FragColor = vec4(vec3(1.0) - color, 1.0); // Invert the color
}
