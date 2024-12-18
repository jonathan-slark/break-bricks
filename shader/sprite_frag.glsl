#version 330 core
#pragma shader_stage(fragment)

in vec2 fragcoords;
out vec4 outcol;

uniform sampler2D tex;

void main()
{
    outcol = texture(tex, fragcoords);
}
