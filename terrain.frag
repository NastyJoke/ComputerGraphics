#version 150

out vec4 outColor;
in vec2 texCoord;
uniform sampler2D tex;
in vec3 exColor;

void main(void)
{
	outColor = exColor[0] * texture(tex, texCoord);
}
