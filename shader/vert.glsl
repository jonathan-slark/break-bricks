#version 330 core
#pragma shader_stage(vertex)

layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 texCoords;
out vec2 fragCoords;
uniform mat4 proj;

void main() {
    fragCoords  = texCoords;
    gl_Position = proj * vec4(pos, 0.0, 1.0);
}
