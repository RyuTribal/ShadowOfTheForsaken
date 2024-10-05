#version 460 core

out vec4 FragColor;

in vec2 TexCoords;

layout(binding = 0) uniform sampler2D screenTexture;

uniform float vignetteStrength = 0.8;

void main()
{
    vec3 color = texture(screenTexture, TexCoords).rgb;
    vec2 uv = TexCoords - 0.5;
    float dist = length(uv) * vignetteStrength;
    color *= smoothstep(1.0, 0.0, dist);

    FragColor = vec4(color, 1.0);
}
