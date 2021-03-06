#version 150

in  vec3 inPosition;
in  vec3 inNormal;
in vec2 inTexCoord;
out vec2 texCoord;
out vec3 exColor;

// NY
uniform mat4 projMatrix;
uniform mat4 mdlMatrix;


void main(void)
{
	// Light
	const vec3 light = normalize(vec3(-1, -1, 1));
	float shade; 
	shade = dot(normalize(inNormal), light); 
	shade = clamp(shade, 0, 1);
	exColor = vec3(shade);

	texCoord = inTexCoord;
	gl_Position = projMatrix * mdlMatrix * vec4(inPosition, 1.0);
}
