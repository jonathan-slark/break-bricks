#version 460 core
#pragma shader_stage(vertex)

layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texcoords>
layout (location = 0) out vec2 texcoords;

layout (location = 0) uniform mat4 model;
layout (location = 1) uniform mat4 projection;

void main()
{
    texcoords = vertex.zw;
    gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);
}
