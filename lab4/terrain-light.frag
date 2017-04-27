#version 150

out vec4 outColor;
in vec3 exColor;
in vec2 texCoord;
uniform sampler2D tex;

void main(void)
{
	outColor = exColor[0] * texture(tex, texCoord);
}
