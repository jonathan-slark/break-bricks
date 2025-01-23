#version 330 core
#pragma shader_stage(fragment)

in vec2 fragcoords;
out vec4 outcol;
uniform sampler2D tex;
uniform vec3 col;

void main()
{
    outcol = vec4(texture(tex, fragcoords).r) * vec4(col, 1.0);
}
