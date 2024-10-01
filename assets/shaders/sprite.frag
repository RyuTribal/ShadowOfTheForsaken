#version 460

in vec4 vertColor;
in vec2 vertTex;
in vec2 spriteCoords;
in vec2 spriteSize;

out vec4 FragColor;

uniform vec4 u_Color;
uniform int u_UsingTexture;
layout(binding = 0) uniform sampler2D u_SpriteTexture;

void main(){
    if(u_UsingTexture == 1){
        vec2 sprite_uv = vertTex * spriteSize;
        sprite_uv += spriteCoords * spriteSize;
        FragColor = texture(u_SpriteTexture, sprite_uv);
    } else{
        FragColor = vertColor;
    }
}