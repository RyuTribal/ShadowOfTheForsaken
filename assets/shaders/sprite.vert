#version 460

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;

out vec4 vertColor;

uniform mat4 u_Transform;

void main(){
    gl_Position = u_Transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);

    vertColor = aColor;
}