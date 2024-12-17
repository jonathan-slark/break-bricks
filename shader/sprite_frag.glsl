#version 330 core
#pragma shader_stage(fragment)

in vec2 fragcoords;
out vec4 outcol;

uniform sampler2D tex;
uniform vec3 colour;

void main()
{
    outcol = vec4(colour, 1.0) * texture(tex, fragcoords);
}
