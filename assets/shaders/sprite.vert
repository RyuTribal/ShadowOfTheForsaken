#version 460

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTex;
layout (location = 3) in vec2 aSpriteCoords;
layout (location = 4) in vec2 aSpriteSize;

out vec4 vertColor;
out vec2 vertTex;
out vec2 spriteCoords;
out vec2 spriteSize;

uniform mat4 u_ViewMatrix;
uniform mat4 u_ProjectionMatrix;

void main(){
    gl_Position = u_ProjectionMatrix * u_ViewMatrix * vec4(aPos, 1.0);

    vertColor = aColor;
    vertTex = aTex; 
    spriteCoords = aSpriteCoords;
    spriteSize = aSpriteSize;
}