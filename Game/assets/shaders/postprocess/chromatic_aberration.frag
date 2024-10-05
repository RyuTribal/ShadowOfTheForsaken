#version 460 core

out vec4 FragColor;

in vec2 TexCoords;

layout(binding = 0) uniform sampler2D screenTexture;

uniform float aberrationStrength = 0.005;

void main()
{
    // Offsets for the different color channels
    vec2 redOffset = vec2(aberrationStrength, 0.0);
    vec2 greenOffset = vec2(-aberrationStrength, aberrationStrength);
    vec2 blueOffset = vec2(0.0, -aberrationStrength);

    // Sample the red, green, and blue channels with slight offsets
    vec3 redChannel = texture(screenTexture, TexCoords + redOffset).rgb;
    vec3 greenChannel = texture(screenTexture, TexCoords + greenOffset).rgb;
    vec3 blueChannel = texture(screenTexture, TexCoords + blueOffset).rgb;

    // Combine the channels to form the final color
    vec3 color = vec3(redChannel.r, greenChannel.g, blueChannel.b);

    // Set the final color with full opacity
    FragColor = vec4(color, 1.0);
}