#version 460

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTex;

out vec4 vertColor;
out vec2 vertTex;

uniform mat4 u_Transform;
uniform mat4 u_ViewMatrix;
uniform mat4 u_ProjectionMatrix;

void main(){
    gl_Position = u_ProjectionMatrix * u_ViewMatrix * u_Transform * vec4(aPos, 1.0);

    vertColor = aColor;
    vertTex = aTex; 
}