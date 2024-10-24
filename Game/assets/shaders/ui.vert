#version 460

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTex;
layout (location = 3) in float aTextureIndex;

out vec4 vertColor;
out vec2 vertTex;
out float textureIndex;

uniform mat4 u_ViewMatrix;
uniform mat4 u_ProjectionMatrix;

void main(){
    gl_Position = vec4(aPos, 1.0);

    vertColor = aColor;
    vertTex = aTex;
    textureIndex = aTextureIndex;
}