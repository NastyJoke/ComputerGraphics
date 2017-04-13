#ifdef __APPLE__
#include <OpenGL/gl3.h>
// Linking hint for Lightweight IDE
// uses framework Cocoa
#endif
#include "../common/Linux/MicroGlut.h"
#include "../common/GL_utilities.h"
#include "../common/loadobj.h"
#include "../common/LoadTGA.h"
#include <math.h>
#include "../common/VectorUtils3.h"

// Globals
// Data would normally be read from files

#define near 1.0
#define far 300.0
#define right 0.5
#define left -0.5
#define top 0.5
#define bottom -0.5

float x, y, z; //camera position
float directionX, directionZ; //camera direction
float angle; //angle of rotation for the camera direction

mat4 frustum_matrix, camera_placement, global_rotation, rotation, translation, time_rotation, move_skybox;
Model *walls, *roof, *blade1, *blade2, *blade3, *blade4, *balcony, *bunny, *teddy, *ground, *skybox;
GLuint grass_tex, skybox_tex, conc_tex;

GLuint program, skybox_program, ground_program; // Reference to shader program
GLfloat t; //time



Point3D lightSourcesColorsArr[] = { {1.0f, 0.0f, 0.0f}, // Red light
                                 {0.0f, 1.0f, 0.0f}, // Green light
                                 {0.0f, 0.0f, 1.0f}, // Blue light
                                 {1.0f, 1.0f, 1.0f} }; // White light

GLfloat specularExponent[] = {10.0, 20.0, 60.0, 5.0};
GLint isDirectional[] = {0,0,1,1};

Point3D lightSourcesDirectionsPositions[] = { {10.0f, 5.0f, 0.0f}, // Red light, positional
                                       {0.0f, 5.0f, 10.0f}, // Green light, positional
                                       {-1.0f, 0.0f, 0.0f}, // Blue light along X
                                       {0.0f, 0.0f, -1.0f} }; // White light along Z




void init(void)
{
	//load textures
	LoadTGATextureSimple("maskros512.tga", &grass_tex);
	LoadTGATextureSimple("SkyBox512.tga", &skybox_tex);
	LoadTGATextureSimple("grass.tga", &conc_tex);

	//load objects
	walls = LoadModelPlus("windmill/windmill-walls.obj");
	balcony = LoadModelPlus("windmill/windmill-balcony.obj");
	roof = LoadModelPlus("windmill/windmill-roof.obj");
	blade1 = LoadModelPlus("windmill/blade.obj");
	blade2 = LoadModelPlus("windmill/blade.obj");
	blade3 = LoadModelPlus("windmill/blade.obj");
	blade4 = LoadModelPlus("windmill/blade.obj");
	ground = LoadModelPlus("ground.obj");
	bunny = LoadModelPlus("../lab2/bunny.obj");
	teddy = LoadModelPlus("../lab2/teddy.obj");
	skybox = LoadModelPlus("skybox.obj");

	dumpInfo();

	// GL inits
	printError("GL inits");
	glClearColor(0.2,0.2,0.6,0);
	glutInitDisplayMode(GLUT_DEPTH);

	// Load and compile shader
	printError("init shader");
	program = loadShaders("lab3-5.vert", "lab3-5.frag");
	skybox_program = loadShaders("lab3-3-skybox.vert", "lab3-3-skybox.frag");
	ground_program = loadShaders("lab3-4-ground.vert", "lab3-4-ground.frag");
	
	//load textures
	glUseProgram(skybox_program);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, skybox_tex);
	glUniform1i(glGetUniformLocation(skybox_program, "texUnit"), 0); // Texture unit 0

	glUseProgram(ground_program);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, grass_tex);
	glUniform1i(glGetUniformLocation(ground_program, "texUnit"), 1); // Texture unit 1
	
	glUseProgram(program);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, conc_tex);
	glUniform1i(glGetUniformLocation(program, "texUnit"), 2); // Texture unit 2
	
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

void display(void) {
	
	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	t = (GLfloat)glutGet(GLUT_ELAPSED_TIME);
	frustum_matrix = frustum(left, right, bottom, top, near, far);
	camera_placement = lookAt(x, y, z, x + directionX, y, z + directionZ, 0, 1, 0);
	move_skybox = T(x, y-0.2f, z);
	
	//skybox
	glUseProgram(skybox_program);
	printError("skybox");
	
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	glUniformMatrix4fv(glGetUniformLocation(skybox_program, "frustum"), 1, GL_TRUE, frustum_matrix.m);
	glUniformMatrix4fv(glGetUniformLocation(skybox_program, "camera"), 1, GL_TRUE, camera_placement.m);
	glUniformMatrix4fv(glGetUniformLocation(skybox_program, "move_skybox"), 1, GL_TRUE, move_skybox.m);
	glUniform1i(glGetUniformLocation(skybox_program, "texUnit"), 0);
	DrawModel(skybox, skybox_program, "in_Position", 0L, "inTexCoord");

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	//ground
	printError("ground");
	glUseProgram(ground_program);
	glUniformMatrix4fv(glGetUniformLocation(ground_program, "camera"), 1, GL_TRUE, camera_placement.m);
	glUniformMatrix4fv(glGetUniformLocation(ground_program, "frustum"), 1, GL_TRUE, frustum_matrix.m);
	glUniform1i(glGetUniformLocation(ground_program, "texUnit"), 1);
	DrawModel(ground, ground_program, "in_Position", 0L, "inTexCoord");
	
	//rotation for whole model
	global_rotation = Ry(t/1000);
	//transaltion applied later to the wings
	translation = T(0,0,0);
	//rotation applied later to the wings
	time_rotation = Rx(0);

	glUseProgram(program);
	glUniform3fv(glGetUniformLocation(program, "lightSourcesDirPosArr"), 4, &lightSourcesDirectionsPositions[0].x);
	glUniform3fv(glGetUniformLocation(program, "lightSourcesColorArr"), 4, &lightSourcesColorsArr[0].x);
	glUniform1fv(glGetUniformLocation(program, "specularExponent"), 4, specularExponent);
	glUniform1iv(glGetUniformLocation(program, "isDirectional"), 4, isDirectional);
	glUniformMatrix4fv(glGetUniformLocation(program, "frustum"), 1, GL_TRUE, frustum_matrix.m);
	glUniformMatrix4fv(glGetUniformLocation(program, "camera"), 1, GL_TRUE, camera_placement.m);
	glUniformMatrix4fv(glGetUniformLocation(program, "global_rotation"), 1, GL_TRUE, global_rotation.m);
	glUniformMatrix4fv(glGetUniformLocation(program, "translation"), 1, GL_TRUE, translation.m);
	glUniformMatrix4fv(glGetUniformLocation(program, "time_rotation"), 1, GL_TRUE, time_rotation.m);

	// walls
	printError("walls");
	glUniform1i(glGetUniformLocation(program, "texUnit"), 2);
	DrawModel(walls, program, "in_Position", "in_Normal", "inTexCoord");

	// roof
	printError("roof");
	DrawModel(roof, program, "in_Position", "in_Normal", "inTexCoord");

	// balcony
	printError("balcony");
	DrawModel(balcony, program, "in_Position", "in_Normal", "inTexCoord");

	//stationary objects do not rotate
	global_rotation = Ry(0);
	glUniformMatrix4fv(glGetUniformLocation(program, "global_rotation"), 1, GL_TRUE, global_rotation.m);

	//bunny
	printError("bunny");
	//translates stationary object
	translation = T(8, 1, 10);
	glUniformMatrix4fv(glGetUniformLocation(program, "translation"), 1, GL_TRUE, translation.m);
	DrawModel(bunny, program, "in_Position", "in_Normal", "inTexCoord");

	//teddy
	printError("teddy");
	//translates stationary object
	translation = T(-10, 1.2f, 5);
	glUniformMatrix4fv(glGetUniformLocation(program, "translation"), 1, GL_TRUE, translation.m);
	DrawModel(teddy, program, "in_Position", "in_Normal", "inTexCoord");

	//unstationary objects do rotate
	global_rotation = Ry(t/1000);
	glUniformMatrix4fv(glGetUniformLocation(program, "global_rotation"), 1, GL_TRUE, global_rotation.m);

	//blades transformations
	translation = T(4.6, 9.3, 0.0);  //translates the blades to the windmill
	time_rotation = Rx(t/1000); //applies rotation to the blades
	glUniformMatrix4fv(glGetUniformLocation(program, "time_rotation"), 1, GL_TRUE, time_rotation.m);
	glUniformMatrix4fv(glGetUniformLocation(program, "translation"), 1, GL_TRUE, translation.m);

	// blade1
	printError("blade1");
	//handles the uniform rotation of the blades
	rotation = Rx(3.141592/2);
	glUniformMatrix4fv(glGetUniformLocation(program, "rotation"), 1, GL_TRUE, rotation.m);
	//glUniform1i(glGetUniformLocation(program, "texUnit"), 3);
	DrawModel(blade1, program, "in_Position", "in_Normal", "inTexCoord");

	// blade2
	printError("blade2");
	//handles the uniform rotation of the blades
	rotation = Rx(3.141592);
	glUniformMatrix4fv(glGetUniformLocation(program, "rotation"), 1, GL_TRUE, rotation.m);
	//glUniform1i(glGetUniformLocation(program, "texUnit"), 3);
	DrawModel(blade2, program, "in_Position", "in_Normal", "inTexCoord");

	// blade3
	printError("blade3");
	//handles the uniform rotation of the blades
	rotation = Rx(3*3.141592/2);
	glUniformMatrix4fv(glGetUniformLocation(program, "rotation"), 1, GL_TRUE, rotation.m);
	//glUniform1i(glGetUniformLocation(program, "texUnit"), 3);
	DrawModel(blade3, program, "in_Position", "in_Normal", "inTexCoord");

	// blade4
	printError("blade4");
	//handles the uniform rotation of the blades
	rotation = Rx(0);
	glUniformMatrix4fv(glGetUniformLocation(program, "rotation"), 1, GL_TRUE, rotation.m);
	//glUniform1i(glGetUniformLocation(program, "texUnit"), 3);
	DrawModel(blade4, program, "in_Position", "in_Normal", "inTexCoord");

	glutSwapBuffers();
}

int main(int argc, char *argv[]) {
	
	x = 0.0f; z = 15.0f; y = 6.0f;
	directionX = sin(angle); directionZ = -cos(angle);
	
	glutInit(&argc, argv);
	glutInitContextVersion(3, 2);
	glutDisplayFunc(display);
	glutInitWindowSize (600, 600);
	glutCreateWindow ("GL3 white triangle example");
	init();
	glutTimerFunc(20, &timer, 0);
	glutMainLoop();
	return 0;
}
