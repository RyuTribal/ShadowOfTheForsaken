#version 460

in vec4 vertColor;
in vec2 vertTex;
in vec2 spriteSize;
in vec2 spriteSegments;
in float tileIndexOffset;

out vec4 FragColor;

uniform vec4 u_Color;
uniform int u_UsingTexture;
layout(binding = 0) uniform sampler2D u_SpriteTexture;
layout(binding = 0, std430) buffer TileBuffer {
    vec2 tileIndices[];
};

void main() {
    if(u_UsingTexture == 1) {
        vec2 segmentCoords = vertTex * spriteSegments;
        vec2 segmentIndex = floor(segmentCoords);
        int tileIndex = int(segmentIndex.y * spriteSegments.x + segmentIndex.x + tileIndexOffset);
        vec2 tileCoords = tileIndices[tileIndex];
        vec2 sprite_uv = fract(segmentCoords);
        sprite_uv *= spriteSize;
        sprite_uv += tileCoords * spriteSize;
        FragColor = texture(u_SpriteTexture, sprite_uv);
    } else {
        FragColor = vertColor;
    }
}