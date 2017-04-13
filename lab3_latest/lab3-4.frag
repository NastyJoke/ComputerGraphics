#version 150

out vec4 out_Color;
in vec2 exTexCoord;
in vec3 exColor;

in mat4 lookAtMatrix;

// Phong
in vec3 normal;
in vec3 surface;
uniform vec3 lightSourcesDirPosArr[4];
uniform vec3 lightSourcesColorArr[4];
uniform float specularExponent[4];
uniform bool isDirectional[4];


uniform vec3 camera_position;


uniform sampler2D texUnit;


void main(void)
{

	vec3 shade = vec3(0,0,0);
	vec3 diffuse = vec3(0,0,0);
	vec3 specular = vec3(0,0,0);

	// Diffuse
	float new_diffuse = 0;
	for(int i = 0; i < 4; i++)
	{
		new_diffuse = dot(normal, lightSourcesDirPosArr[i]);
		new_diffuse = max(0.0, new_diffuse);
		diffuse += new_diffuse * lightSourcesColorArr[i];
	}

	// Specular
	float new_specular;
	for(int i = 0; i < 4; i++)
	{
		vec3 refl;

		if (isDirectional[i])
		{
			refl = reflect(lightSourcesDirPosArr[i], normal);
		}
		else
		{
			refl = reflect(normalize(surface-lightSourcesDirPosArr[i]), normal);
		}

		vec3 v = normalize(camera_position-surface);

		new_specular = dot(refl, v);
		if (new_specular > 0.0)
		{
			new_specular = pow(new_specular, specularExponent[i]);
		}
		new_specular = max(new_specular, 0.0);
		specular += new_specular * lightSourcesColorArr[i];
	}



	out_Color = vec4(diffuse+specular,1.0);
	//out_Color = vec4(exColor,1.0);
}
