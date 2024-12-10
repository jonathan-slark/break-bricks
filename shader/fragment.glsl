#version 460 core
#pragma shader_stage(fragment)

layout(location = 0) out vec4 outcolour;

void main()
{
    outcolour = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}
