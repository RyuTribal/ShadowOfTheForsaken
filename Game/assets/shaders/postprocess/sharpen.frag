#version 460 core

out vec4 FragColor;

in vec2 TexCoords;

layout(binding = 0) uniform sampler2D screenTexture;

void main()
{
    vec2 tex_offset = 1.0 / textureSize(screenTexture, 0);
    vec3 center = texture(screenTexture, TexCoords).rgb;
    vec3 left = texture(screenTexture, TexCoords - vec2(tex_offset.x, 0.0)).rgb;
    vec3 right = texture(screenTexture, TexCoords + vec2(tex_offset.x, 0.0)).rgb;
    vec3 up = texture(screenTexture, TexCoords + vec2(0.0, tex_offset.y)).rgb;
    vec3 down = texture(screenTexture, TexCoords - vec2(0.0, tex_offset.y)).rgb;
    vec3 result = center * 5.0 - (left + right + up + down);

    FragColor = vec4(result, 1.0);
}
