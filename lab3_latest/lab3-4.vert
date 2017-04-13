#version 150

in  vec3 in_Normal;
in  vec3 in_Position;
in  vec2 inTexCoord;
out vec4 frac_color;



uniform mat4 translation;
uniform mat4 global_rotation;
uniform mat4 time_rotation;
uniform mat4 rotation;
uniform mat4 camera;
uniform mat4 frustum;
mat3 normalRot;
vec3 normal;

out vec3 camera_position;
in vec3 xyz;

out vec3 exColor;
out vec2 exTexCoord;


void main() {

  camera_position = xyz;

  // Texture
  exTexCoord = inTexCoord;

  // Light
  normalRot = mat3(global_rotation);
  const vec3 light = normalize(vec3(2, 2, -3));
  float shade;
  normal = normalRot * in_Normal;
  shade = dot(normalize(normal), light);
  shade = clamp(shade, 0, 1);
  exColor = vec3(shade);

  // Model to View
  gl_Position = frustum * camera  * global_rotation * translation * time_rotation * rotation * vec4(in_Position, 1.0);

}
