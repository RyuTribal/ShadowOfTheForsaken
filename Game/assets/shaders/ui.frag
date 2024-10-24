#version 460

in vec4 vertColor;
in vec2 vertTex;
in float textureIndex;

out vec4 FragColor;

uniform sampler2D u_Textures[16];

void main() {
    int texture_index = int(textureIndex);
    if(texture_index > -1) {
        FragColor = texture(u_Textures[texture_index], vertTex);
    } else {
        FragColor = vertColor;
    }
}