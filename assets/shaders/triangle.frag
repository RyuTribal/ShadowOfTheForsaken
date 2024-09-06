#version 460

in vec4 vertColor;

out vec4 FragColor;

uniform vec4 u_Color;

void main(){
    FragColor = u_Color;
}