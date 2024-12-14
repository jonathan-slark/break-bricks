#version 460 core
#pragma shader_stage(fragment)

layout (location = 0) in vec2 texcoords;
layout (location = 0) out vec4 outcol;

layout (location = 2) uniform sampler2D tex;

void main()
{
    outcol = texture(tex, texcoords);
    /* outcol = vec4(1.0f, 0.0f, 0.0f, 1.0f); */
}
