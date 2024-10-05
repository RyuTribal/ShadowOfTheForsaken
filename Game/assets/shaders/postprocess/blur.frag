#version 460 core

out vec4 FragColor;

in vec2 TexCoords;

layout(binding = 0) uniform sampler2D screenTexture;

uniform float blurRadius = 1.0; // Control the blur amount

void main()
{
    vec2 tex_offset = 1.0 / textureSize(screenTexture, 0);

    vec3 result = texture(screenTexture, TexCoords).rgb * 0.227027;
    result += texture(screenTexture, TexCoords + vec2(tex_offset.x * blurRadius, 0.0)).rgb * 0.1945946;
    result += texture(screenTexture, TexCoords - vec2(tex_offset.x * blurRadius, 0.0)).rgb * 0.1945946;
    result += texture(screenTexture, TexCoords + vec2(0.0, tex_offset.y * blurRadius)).rgb * 0.1216216;
    result += texture(screenTexture, TexCoords - vec2(0.0, tex_offset.y * blurRadius)).rgb * 0.1216216;

    FragColor = vec4(result, 1.0);
}
