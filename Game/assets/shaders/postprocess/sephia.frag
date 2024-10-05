#version 460 core

out vec4 FragColor;

in vec2 TexCoords;

layout(binding = 0) uniform sampler2D screenTexture;

void main()
{
    vec3 color = texture(screenTexture, TexCoords).rgb;
    
    vec3 sepiaColor;
    sepiaColor.r = dot(color, vec3(0.393, 0.769, 0.189));
    sepiaColor.g = dot(color, vec3(0.349, 0.686, 0.168));
    sepiaColor.b = dot(color, vec3(0.272, 0.534, 0.131));

    FragColor = vec4(sepiaColor, 1.0);
}