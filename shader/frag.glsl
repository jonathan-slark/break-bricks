#version 330 core
#pragma shader_stage(fragment)

in vec2 fragCoords;
out vec4 outCol;
uniform sampler2D tex;
uniform vec3 col;
uniform bool isFont;

void main()
{
    if (isFont) {
	outCol = vec4(texture(tex, fragCoords).r) * vec4(col, 1.0);
    } else {
	outCol = texture(tex, fragCoords);
    }
}
