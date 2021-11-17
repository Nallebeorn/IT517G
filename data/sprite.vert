#version 330 core

layout (location = 0) in vec2 aPos;

out vec2 vTexCoord;

void main()
{
    vTexCoord = aPos + 0.5;
    vTexCoord.y = 1.0 - vTexCoord.y;
    vTexCoord = clamp(vTexCoord, 0.0, 1.0);

    gl_Position = vec4(aPos.xy, 0.0, 1.0);
}

