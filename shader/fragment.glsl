#version 460 core
#pragma shader_stage(fragment)

layout (location = 0) in vec2 texvert;
layout (location = 0) out vec4 outcol;

layout (location = 2) uniform sampler2D tex;
layout (location = 3) uniform vec3 col;

void main()
{
    outcol = vec4(col, 1.0) * texture(tex, texvert);
}
