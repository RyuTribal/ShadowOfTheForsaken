#version 460

in vec2 TexCoords;
out vec4 FragColor;

layout(binding = 0) uniform sampler2D u_BackgroundTexture;

void main() {
    FragColor = texture(u_BackgroundTexture, TexCoords);
}