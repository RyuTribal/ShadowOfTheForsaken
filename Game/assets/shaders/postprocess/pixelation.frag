#version 460 core

out vec4 FragColor;

in vec2 TexCoords;

layout(binding = 0) uniform sampler2D screenTexture;

uniform float pixelSize = 0.003; // Control the pixel size

void main()
{
    vec2 pixelatedUV = vec2(floor(TexCoords.x / pixelSize) * pixelSize, 
                            floor(TexCoords.y / pixelSize) * pixelSize);
    vec3 color = texture(screenTexture, pixelatedUV).rgb;

    FragColor = vec4(color, 1.0);
}
