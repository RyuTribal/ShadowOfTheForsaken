#version 460

in vec4 vertColor;
in vec2 vertTex;

out vec4 FragColor;

uniform vec4 u_Color;
layout(binding = 0) uniform sampler2D u_SpriteTexture;

void main(){
    FragColor = texture(u_SpriteTexture, vertTex);
    
}