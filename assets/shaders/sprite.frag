#version 460

in vec4 vertColor;
in vec2 vertTex;

out vec4 FragColor;

uniform vec4 u_Color;
layout(binding = 0) uniform sampler2D u_spriteTexture;

void main(){
    FragColor = texture(ourTexture, TexCoord);
    
}