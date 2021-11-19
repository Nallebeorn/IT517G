#version 330 core

uniform vec2 uViewportSize;

layout (location = 0) in vec2 aPos;

out vec2 vTexCoord;

void main()
{
    // Calculate UV from vertex positions
    vTexCoord = aPos + 0.5;
    vTexCoord.y = 1.0 - vTexCoord.y;
    vTexCoord = clamp(vTexCoord, 0.0, 1.0);

    // Input position is pixel-coordinates (0,0) at top-left, convert to clip-space
    vec2 pos = aPos;
    pos = pos * 16;
    pos.x += 1.5;
    pos = pos / (uViewportSize * 0.5);
    pos.x -= 1.0;
    pos.y += 1.0;

    gl_Position = vec4(pos, 0.0, 1.0);
}

