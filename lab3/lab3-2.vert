#version 150

in  vec3 in_Normal;
in  vec3 in_Position;
in  vec2 inTexCoord;
out vec4 frac_color;


uniform mat4 blade_translation;
uniform mat4 rot;
uniform mat4 blade_time_rot;
uniform mat4 blade_place;
uniform mat4 camera;
uniform mat4 frustum;
mat3 normalRot;
vec3 normal;


out vec3 exColor;
//out vec2 exTexCoord;


void main() {
	
	// Texture
	//exTexCoord = inTexCoord;
	
	// Light
	normalRot = mat3(rot);
	const vec3 light = normalize(vec3(2, 2, -3));
	float shade; 
	normal = normalRot * in_Normal;
	shade = dot(normalize(normal), light); 
	shade = clamp(shade, 0, 1);
	exColor = vec3(shade);
	
	
	
	// Model to View
	gl_Position = frustum * camera  * rot * blade_translation * blade_time_rot * blade_place * vec4(in_Position, 1.0);

}
