// Lab 4, terrain generation

#ifdef __APPLE__
	#include <OpenGL/gl3.h>
	// Linking hint for Lightweight IDE
	// uses framework Cocoa
#endif
#include "MicroGlut.h"
#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "loadobj.h"
#include "LoadTGA.h"

mat4 projectionMatrix;


float x, y, z; //camera position
float directionX, directionZ; //camera direction
float angle; //angle of rotation for the camera direction
int texWidth;
static float speed = 0.2f;
vec3 posObj; //position of the moving object
float t=0, t_cam=0;

Model* GenerateTerrain(TextureData *tex)
{
	int vertexCount = tex->width * tex->height;
	int triangleCount = (tex->width-1) * (tex->height-1) * 2;
	int x, z;

	GLfloat *vertexArray = malloc(sizeof(GLfloat) * 3 * vertexCount);
	GLfloat *normalArray = malloc(sizeof(GLfloat) * 3 * vertexCount);
	GLfloat *texCoordArray = malloc(sizeof(GLfloat) * 2 * vertexCount);
	GLuint *indexArray = malloc(sizeof(GLuint) * triangleCount*3);

	printf("bpp %d\n", tex->bpp);
	for (x = 0; x < tex->width; x++)
		for (z = 0; z < tex->height; z++)
		{

			texWidth = tex->width;

// Vertex array. You need to scale this properly
			vertexArray[(x + z * tex->width)*3 + 0] = x / 1.0;
			vertexArray[(x + z * tex->width)*3 + 1] = tex->imageData[(x + z * tex->width) * (tex->bpp/8)] / 20.0;
			vertexArray[(x + z * tex->width)*3 + 2] = z / 1.0;
// Normal vectors. You need to calculate these.
			vec3 left;
			vec3 us;
			vec3 top;
			us =(vec3) {vertexArray[(x + z * tex->width)*3 + 0],
                  vertexArray[(x + z * tex->width)*3 + 1],
                  vertexArray[(x + z * tex->width)*3 + 2]};
			if(x - 1 < 0){
				left =(vec3){0, 1, 0};
			}
			else{
				left =(vec3) {vertexArray[(x-1 + z * tex->width)*3 + 0],
                      vertexArray[(x-1 + z * tex->width)*3 + 1],
                      vertexArray[(x-1 + z * tex->width)*3 + 2]};
			}
			if (z - 1 < 0){
				top=(vec3){0, 1, 0};
			}
			else {
				top = (vec3){vertexArray[(x + (z-1) * tex->width)*3 + 0],
                     vertexArray[(x + (z-1) * tex->width)*3 + 1],
                     vertexArray[(x + (z-1) * tex->width)*3 + 2]};
			}
			vec3 leftV = VectorSub(left, us);
			vec3 topV = VectorSub(top, us);
			vec3 normal = Normalize(CrossProduct(leftV, topV));

			normalArray[(x + z * tex->width)*3 + 0] = normal.x;
			normalArray[(x + z * tex->width)*3 + 1] = normal.y;
			normalArray[(x + z * tex->width)*3 + 2] = normal.z;
// Texture coordinates. You may want to scale them.
			texCoordArray[(x + z * tex->width)*2 + 0] = x;//(float)x / tex->width;
			texCoordArray[(x + z * tex->width)*2 + 1] = z;//(float)z / tex->height;
		}
	for (x = 0; x < tex->width-1; x++)
		for (z = 0; z < tex->height-1; z++)
		{
		// Triangle 1
			indexArray[(x + z * (tex->width-1))*6 + 0] = x + z * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 1] = x + (z+1) * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 2] = x+1 + z * tex->width;
		// Triangle 2
			indexArray[(x + z * (tex->width-1))*6 + 3] = x+1 + z * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 4] = x + (z+1) * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 5] = x+1 + (z+1) * tex->width;
		}

	// End of terrain generation

	// Create Model and upload to GPU:

	Model* model = LoadDataToModel(
			vertexArray,
			normalArray,
			texCoordArray,
			NULL,
			indexArray,
			vertexCount,
			triangleCount*3);

	return model;
}







// vertex array object
Model *m, *m2, *tm, *octagon;
// Reference to shader program
GLuint program;
GLuint tex1, tex2;
TextureData ttex; // terrain



GLfloat calculateHeight(Model *groundMap, vec3 position){
	GLfloat height = 0;

	height = groundMap->vertexArray[((int)position.x + (int)position.z * texWidth)*3 + 1];

	return height;
}


float interpolate(float speed, float smoothing, vec3 pos, float t){

	float m = 0;
	vec3 newpos = pos;
	//new position
	newpos.x = pos.x+(speed*smoothing);
	newpos.z = pos.z+(speed*smoothing);
	//calculate slope
	m = 1.3*(calculateHeight(tm, newpos)-t);
	pos.y = m*speed + t;
	t = pos.y;
	return pos.y;

}



void init(void)
{
	// GL inits
	glClearColor(0.2,0.2,0.5,0);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	printError("GL inits");

	projectionMatrix = frustum(-0.1, 0.1, -0.1, 0.1, 0.2, 50.0);

	// Load and compile shader
	program = loadShaders("terrain.vert", "terrain.frag");
	glUseProgram(program);
	printError("init shader");

	glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
	glUniform1i(glGetUniformLocation(program, "tex"), 0); // Texture unit 0
	LoadTGATextureSimple("grass.tga", &tex1);

// Load terrain data

	LoadTGATextureData("fft-terrain.tga", &ttex);
	tm = GenerateTerrain(&ttex);
	octagon = LoadModelPlus("octagon.obj");
	posObj.x = 10;
	posObj.z = 50;
	posObj.y = 0;
	posObj.y=calculateHeight(tm, posObj);
	t = calculateHeight(tm, posObj);
	printError("init terrain");
}

void display(void)
{

	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat4 total, modelView, camMatrix, transMatrix;

	printError("pre display");

	glUseProgram(program);

	// Build matrix

	transMatrix = T(0,0,0);

	camMatrix = lookAt(x, y, z, x + directionX, y, z + directionZ, 0, 1, 0);
	modelView = IdentityMatrix();
	total = Mult(camMatrix, modelView);
	total = Mult(total, transMatrix);
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);

	glBindTexture(GL_TEXTURE_2D, tex1);		// Bind Our Texture tex1
	DrawModel(tm, program, "inPosition", "inNormal", "inTexCoord");

	printError("display 2");

	float objSpeed=0.1f;
	//make the model move
	posObj.x= posObj.x + objSpeed;
	posObj.y = interpolate(objSpeed, 5, posObj, t);
	transMatrix = T(posObj.x, posObj.y, posObj.z);
	total = Mult(camMatrix, modelView);
	total = Mult(total, transMatrix);
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
	t = posObj.y;
	DrawModel(octagon, program, "inPosition", "inNormal", "inTexCoord");



	glutSwapBuffers();
}

void timer(int i)
{
	glutTimerFunc(20, &timer, i);
	glutPostRedisplay();


	if (glutKeyIsDown('w')){ //move camera forward
		x += directionX * 0.5f;
		z += directionZ * 0.5f;
	}

	if (glutKeyIsDown('s')){ //move camera backwards
		x -= directionX * 0.5f;
		z -= directionZ * 0.5f;
	}

	if (glutKeyIsDown('a')){ //rotate camera left around Y axis
		angle -= 0.05f;
		directionX = sin(angle);
		directionZ = -cos(angle);
	}

	if (glutKeyIsDown('d')){ //rotate camera right around Y axis
		angle += 0.05f;
		directionX = sin(angle);
		directionZ = -cos(angle);
	}

	if (glutKeyIsDown(32)){ // up
		y += 0.1f;
	}


	if (glutKeyIsDown('c')){ // down
		y -= 0.1f;
	}
}

void mouse(int x, int y)
{
}

int main(int argc, char **argv)
{
	angle = 2.0f;
	x = 0.0f; z = 1.0f; y = 3.0f;
    directionX = sin(angle); directionZ = -cos(angle);


	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
	glutInitContextVersion(3, 2);
	glutInitWindowSize (600, 600);
	glutCreateWindow ("TSBK07 Lab 4");
	glutDisplayFunc(display);
	init ();
	glutTimerFunc(20, &timer, 0);

	glutPassiveMotionFunc(mouse);

	glutMainLoop();
	exit(0);
}
