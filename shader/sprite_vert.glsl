#version 460 core
#pragma shader_stage(vertex)

layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 texcoords;
layout (location = 0) out vec2 fragtexcoords;

layout (location = 0) uniform mat4 model;
layout (location = 1) uniform mat4 proj;

void main()
{
    fragtexcoords = texcoords;
    gl_Position = proj * model * vec4(pos, 0.0, 1.0);
}
