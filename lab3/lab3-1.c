// Lab 1-1.
// This is the same as the first simple example in the course book,
// but with a few error checks.
// Remember to copy your file to a new on appropriate places during the lab so you keep old results.
// Note that the files "lab1-1.frag", "lab1-1.vert" are required.

// Should work as is on Linux and Mac. MS Windows needs GLEW or glee.
// See separate Visual Studio version of my demos.
#ifdef __APPLE__
	#include <OpenGL/gl3.h>
	// Linking hint for Lightweight IDE
	// uses framework Cocoa
#endif
#include "MicroGlut.h"
#include "GL_utilities.h"
#include "loadobj.h"
#include "../common/LoadTGA.h"
#include <math.h>
#include "VectorUtils3.h"

// Globals
// Data would normally be read from files

#define near 1.0
#define far 300.0
#define right 2
#define left -2
#define top 2
#define bottom -2

double x, y, z;

mat4 translation_matrix, total, frustum_matrix, camera_placement, rot, rot2, blade_rot, blade_time_rot;

Model *walls, *roof, *blade1, *blade2, *blade3, *blade4, *balcony;

GLuint tex;
GLuint texBody;

// Reference to shader program
GLuint program;

GLfloat t;



void init(void)
{
	
	//LoadTGATextureSimple("../lab2/models/LittleNell/Hawk/girl-head.tga", &tex);
	//LoadTGATextureSimple("../lab2/models/LittleNell/Hawk/girl-body.tga", &texBody);

	walls = LoadModelPlus("windmill/windmill-walls.obj");
	balcony = LoadModelPlus("windmill/windmill-balcony.obj");
	roof = LoadModelPlus("windmill/windmill-roof.obj");
	blade1 = LoadModelPlus("windmill/blade.obj");
	blade2 = LoadModelPlus("windmill/blade.obj");
	blade3 = LoadModelPlus("windmill/blade.obj");
	blade4 = LoadModelPlus("windmill/blade.obj");
	
	
	dumpInfo();

	// GL inits
	glClearColor(0.2,0.2,0.6,0);
	glutInitDisplayMode(GLUT_DEPTH);
	glEnable(GL_DEPTH_TEST);
	printError("GL inits");

	// Load and compile shader
	program = loadShaders("lab3-1.vert", "lab3-1.frag");
	printError("init shader");
	
    
    /*glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glUniform1i(glGetUniformLocation(program, "texUnit"), 0); // Texture unit 0
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texBody);*/
	
}

void OnTimer(int value)
{
    glutPostRedisplay();
    glutTimerFunc(20, &OnTimer, value);
    
    if (glutKeyIsDown('a'))
		x += 0.1;
	 
	if (glutKeyIsDown('d'))
		x -= 0.1;
	 
	if (glutKeyIsDown('w'))
		y += 0.1;
	 
	if (glutKeyIsDown('s'))
		y -= 0.1;
		
	if (glutKeyIsDown('r'))
		z += 0.1;
	 
	if (glutKeyIsDown('f'))
		z -= 0.1;
		
    printf("%f, %f, %f\n", x, y, z);
}

void display(void)
{    
    t = (GLfloat)glutGet(GLUT_ELAPSED_TIME);

	translation_matrix = T(0,0,0);
	
	frustum_matrix = frustum(left, right, bottom, top, near, far);
	
	camera_placement = lookAt(0, 4, -10,
							  0, 5, 0,
							  0, 1, 0);

	rot = Ry(t/1000);
	blade_time_rot = Rx(0);
	
	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glUniformMatrix4fv(glGetUniformLocation(program, "blade_translation"), 1, GL_TRUE, translation_matrix.m);
	glUniformMatrix4fv(glGetUniformLocation(program, "frustum"), 1, GL_TRUE, frustum_matrix.m);
	glUniformMatrix4fv(glGetUniformLocation(program, "camera"), 1, GL_TRUE, camera_placement.m);
	glUniformMatrix4fv(glGetUniformLocation(program, "rot"), 1, GL_TRUE, rot.m);
	glUniformMatrix4fv(glGetUniformLocation(program, "blade_time_rot"), 1, GL_TRUE, blade_time_rot.m);
	
	
	// walls
	printError("walls");
    //glUniform1i(glGetUniformLocation(program, "texUnit"), 0);
    DrawModel(walls, program, "in_Position", "in_Normal", "inTexCoord");
    
	// roof
	printError("roof");
    //glUniform1i(glGetUniformLocation(program, "texUnit"), 1);
    DrawModel(roof, program, "in_Position", "in_Normal", "inTexCoord");
    
    // balcony
	printError("balcony");
    //glUniform1i(glGetUniformLocation(program, "texUnit"), 2);
    DrawModel(balcony, program, "in_Position", "in_Normal", "inTexCoord");
    
    
    translation_matrix = T(4.6, 9.3, 0.0);
    
    blade_time_rot = Rx(t/1000);
	glUniformMatrix4fv(glGetUniformLocation(program, "blade_time_rot"), 1, GL_TRUE, blade_time_rot.m);
	glUniformMatrix4fv(glGetUniformLocation(program, "blade_translation"), 1, GL_TRUE, translation_matrix.m);
    
    // blade1
	printError("blade1");
	blade_rot = Rx(3.141592/2);
	glUniformMatrix4fv(glGetUniformLocation(program, "blade_place"), 1, GL_TRUE, blade_rot.m);
	glUniformMatrix4fv(glGetUniformLocation(program, "translation_matrix"), 1, GL_TRUE, translation_matrix.m);
    //glUniform1i(glGetUniformLocation(program, "texUnit"), 3);
    DrawModel(blade1, program, "in_Position", "in_Normal", "inTexCoord");
    // blade2
    blade_rot = Rx(3.141592);
	glUniformMatrix4fv(glGetUniformLocation(program, "blade_place"), 1, GL_TRUE, blade_rot.m);
	glUniformMatrix4fv(glGetUniformLocation(program, "rot"), 1, GL_TRUE, rot.m);
	printError("blade2");
    //glUniform1i(glGetUniformLocation(program, "texUnit"), 3);
    DrawModel(blade2, program, "in_Position", "in_Normal", "inTexCoord");
    // blade3
    blade_rot = Rx(3*3.141592/2);
	glUniformMatrix4fv(glGetUniformLocation(program, "blade_place"), 1, GL_TRUE, blade_rot.m);    
	printError("blade3");
    //glUniform1i(glGetUniformLocation(program, "texUnit"), 3);
    DrawModel(blade3, program, "in_Position", "in_Normal", "inTexCoord");
    // blade4
    blade_rot = Rx(0);
	glUniformMatrix4fv(glGetUniformLocation(program, "blade_place"), 1, GL_TRUE, blade_rot.m);    
	printError("blade4");
    //glUniform1i(glGetUniformLocation(program, "texUnit"), 3);
    DrawModel(blade4, program, "in_Position", "in_Normal", "inTexCoord");

	
	glutSwapBuffers();
}

int main(int argc, char *argv[])
{
	x = 0; y = 0; z = 0;
	glutInit(&argc, argv);
	glutInitContextVersion(3, 2);
	glutCreateWindow ("GL3 white triangle example");
	glutDisplayFunc(display); 
	init ();
	glutTimerFunc(20, &OnTimer, 0);
	glutMainLoop();
	return 0;
}
