#version 460 core
#pragma shader_stage(vertex)

layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 texcoords;
out vec2 fragcoords;

uniform mat4 model;
uniform mat4 proj;

void main() {
    fragcoords = texcoords;
    gl_Position = proj * model * vec4(pos, 0.0, 1.0);
}
