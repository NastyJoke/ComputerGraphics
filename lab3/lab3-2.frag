#version 150

out vec4 out_Color;
//in vec2 exTexCoord;
//in vec4 frac_color;
in vec3 exColor;

uniform sampler2D texUnit;


void main(void)
{
	//out_Color = exColor[0]*texture(texUnit, exTexCoord);
	out_Color = vec4(exColor,1.0);
}
