#version 460 core
#pragma shader_stage(vertex)

/* vert: <vec2 pos, vec2 texvert> */
layout (location = 0) in vec4 vert;
layout (location = 0) out vec2 texvert;

layout (location = 0) uniform mat4 model;
layout (location = 1) uniform mat4 proj;

void main()
{
    texvert = vert.zw;
    gl_Position = proj * model * vec4(vert.xy, 0.0, 1.0);
}
