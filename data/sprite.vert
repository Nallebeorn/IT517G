#version 330 core

uniform vec2 uViewportSize;
uniform vec2 uAtlasSize;

layout (location = 0) in vec2 aVertexPos;
layout (location = 1) in vec2 aObjectPos;
layout (location = 2) in vec4 aSpriteRect; // .xy = pixel position within atlas, .zw = pixel width/height within atlas

out vec2 vTexCoord;

void main()
{
    // Calculate UV from vertex positions
    vTexCoord = aVertexPos + 0.5;
    vTexCoord.y = 1.0 - vTexCoord.y;
    vTexCoord = clamp(vTexCoord, 0.0, 1.0); 
    vTexCoord *= aSpriteRect.zw / uAtlasSize;
    vTexCoord += aSpriteRect.xy * (1.0 / uAtlasSize);

    // Input position is pixel-coordinates with (0,0) as top-left.
    // This is converted to clip space.
    vec2 pos = aVertexPos;
    pos = pos * aSpriteRect.zw;
    pos = pos + vec2(aObjectPos.x, -aObjectPos.y);
    pos = floor(pos);
    pos = pos / (uViewportSize * 0.5);
    pos.x -= 1.0;
    pos.y += 1.0;

    gl_Position = vec4(pos, 0.0, 1.0);
}

