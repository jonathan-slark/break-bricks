#version 460 core
#pragma shader_stage(fragment)

layout (location = 0) in vec2 texcoords;
layout (location = 0) out vec4 color;

layout (location = 0) uniform sampler2D image;
layout (location = 1) uniform vec3 spritecolor;

void main()
{
    color = vec4(spritecolor, 1.0) * texture(image, texcoords);
}
