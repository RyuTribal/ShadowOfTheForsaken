#version 460

in vec4 vertColor;
in vec2 vertTex;

out vec4 FragColor;

uniform vec4 u_Color;
uniform int u_UsingTexture;
layout(binding = 0) uniform sampler2D u_SpriteTexture;

void main(){
    if(u_UsingTexture == 1){
        FragColor = texture(u_SpriteTexture, vertTex);
    } else{
        FragColor = vertColor;
    }
    
}