//#include "SDL.h"
#include "GL\glut.h"
#include <GL\GL.h>
#include <GL\glfw.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <string>
#include <istream>

using namespace std;

const int width = 800;
const int height = 600;
float angle = 0.0f, red = 1.0f, blue = 1.0f, green = 1.0f;
bool* keyStates = new bool[256]; // Create an array of boolean values of length 256
bool* keySpecialStates = new bool[246]; 
bool  fullscreen = false, leftMouseDown = false, rightMouseDown; 
float xLocation = 0.0f; // Keep track of our position on the x axis.
float yLocation = 0.0f; // Keep track of our position on the y axis.
float zLocation = -10.0f;
float yRotationAngle = 0.0f; // The angle of rotation for our object
float xRotationAngle = 0.0f; // The angle of rotation for our object
float xdiff = 0.0f;
float ydiff = 0.0f;
float zdiff = 0.0f;
int frameCount = 0;
int frame=0,time,timebase=0;
char s[50];
GLuint my_buffer;

GLvoid *font_style = GLUT_BITMAP_TIMES_ROMAN_10;

void pressNormalKeys(unsigned char key, int x, int y) {keyStates[key] = true; }
void releaseNormalKeys(unsigned char key, int x, int y) {keyStates[key] = false; }
void pressSpecialKeys(int key, int x, int y) {keySpecialStates[key] = true;}
void releaseSpecialKeys(int key, int x, int y) {keySpecialStates[key] = false;}
void printw (float x, float y, float z, char* format, ...);

void keyOperations (void) { 
	if (keyStates[27]) {exit(1);}
}	
void specialKeyOperations(void){
	if(keySpecialStates[GLUT_KEY_F1 ]){
		fullscreen = !fullscreen;
		if (fullscreen)
			glutFullScreen();
		else{
			glutReshapeWindow(width, height);
			glutPositionWindow(50, 50);
		}
	}
	if(keySpecialStates[GLUT_KEY_F2 ]){
		red = 1.0;
		green = 0.0;
		blue = 0.0;
	}
	if(keySpecialStates[GLUT_KEY_F3 ]){
		red = 0.0;
		green = 1.0;
		blue = 0.0;
	}
	if(keySpecialStates[GLUT_KEY_F4 ]){
		red = 0.0;
		green = 0.0;
		blue = 1.0;
	}
	if(keySpecialStates[GLUT_KEY_HOME]){
		zLocation = -10.0f;
		xLocation = 0.0f;
		yLocation = 0.0f;
		xRotationAngle = 0.0f;
		yRotationAngle = 0.0f;
		glutPostRedisplay();
	}
	if(keySpecialStates[GLUT_KEY_LEFT]){
		//cout<< "Left is press."<< endl;

		if (xLocation < -3.0f) ;
		else
			xLocation -= 0.0005f; // Move up along our xLocation

	}
	if(keySpecialStates[GLUT_KEY_RIGHT]){
		//cout<< "Right is press."<< endl;
		if (xLocation > 3.0f) ;
		else
			xLocation += 0.0005f; // Move up along our yLocation


	}
	if(keySpecialStates[GLUT_KEY_UP]){

		if (yLocation > 3.0f) ;
		else
			yLocation += 0.0005f; // Move up along our yLocation


	}
	if(keySpecialStates[GLUT_KEY_DOWN]){
		if (yLocation < -3.0f) ;
		else
			yLocation -= 0.0005f; // Move up along our yLocation
	}


}
void mouse(int button, int state, int x, int y)
{
	switch(button){
	case GLUT_LEFT_BUTTON:
		leftMouseDown = true;
		rightMouseDown = false;
		xdiff = x - yRotationAngle;
		ydiff = -y + xRotationAngle;
		break;
	case GLUT_RIGHT_BUTTON:
		leftMouseDown = false;
		rightMouseDown = true;
		zdiff = y - yLocation;
		break;
	default:
		leftMouseDown = false;
		rightMouseDown = false;
	}
}

void mouseMotion(int x, int y)
{
	if (leftMouseDown)
	{
		//cout<<"rotate"<<endl;
		yRotationAngle = x - xdiff;
		xRotationAngle = y + ydiff;
		
		glutPostRedisplay();
	}
	if (rightMouseDown){
		//cout<<"zoom"<<endl;
		zLocation = y - zdiff;
		glutPostRedisplay();
	}
}

void renderBitmapString(
	float x,
	float y,
	float z,
	void *font,
	char *string) {

		char *c;
		glRasterPos3f(x, y,z);
		for (c=string; *c != '\0'; c++) {
			glutBitmapCharacter(font, *c);
		}
}
void renderStrokeFontString(
	float x,
	float y,
	float z,
	void *font,
	char *string) {  

		char *c;
		glPushMatrix();
		glTranslatef(x, y,z);
		glScalef(0.002f, 0.002f, 0.002f);
		for (c=string; *c != '\0'; c++) {
			glutStrokeCharacter(font, *c);
		}
		glPopMatrix();
}
void restorePerspectiveProjection() {

	glMatrixMode(GL_PROJECTION);
	// restore previous projection matrix
	glPopMatrix();

	// get back to modelview mode
	glMatrixMode(GL_MODELVIEW);
}
void setOrthographicProjection() {

	// switch to projection mode
	glMatrixMode(GL_PROJECTION);

	// save previous matrix which contains the
	//settings for the perspective projection
	glPushMatrix();

	// reset matrix
	glLoadIdentity();

	// set a 2D orthographic projection
	gluOrtho2D(0, width, height, 0);

	// switch back to modelview mode
	glMatrixMode(GL_MODELVIEW);
}
void renderPrimitive (void) {
	glColor3f(0.0f, 0.0f, 1.0f); // Set the colour of the square to blue

	glBegin(GL_QUADS); // Start drawing a quad primitive
	glVertex3f(-1.0f, -1.0f, 0.0f); // The bottom left corner
	glVertex3f(-1.0f, 1.0f, 0.0f); // The top left corner
	glVertex3f(1.0f, 1.0f, 0.0f); // The top right corner
	glVertex3f(1.0f, -1.0f, 0.0f); // The bottom right corner
	glEnd();
}

void calcFPS(){
	// Code to compute frames per second
	frame++;

	time=glutGet(GLUT_ELAPSED_TIME);
	if (time - timebase > 1000) {
		sprintf(s,"FPS:%4.2f",
			frame*1000.0/(time-timebase));
		timebase = time;
		frame = 0;
	}

	// Code to display a string (fps) with bitmap fonts
	setOrthographicProjection();

	glPushMatrix();
	glLoadIdentity();
	renderBitmapString(5,30,0,GLUT_BITMAP_TIMES_ROMAN_24,s);
	glPopMatrix();
	restorePerspectiveProjection();
}

void drawSky(void){
	int i;
	GLfloat ang, x, y, z = -50;
	srand(54654);
	// Clear Color and Depth Buffers
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glColor3f(red,green,blue);
	glBegin(GL_LINES);
	for(int i = 0; i< 100; i++){
		glVertex3f(rand()%width,rand()%height,rand()%100);
	}
	glEnd();

	glutSwapBuffers();

}


void display (void) {
	keyOperations();
	specialKeyOperations();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Clear the background of our window to red
	glClear(GL_COLOR_BUFFER_BIT); //Clear the color buffer (more buffers later on)
	glLoadIdentity(); // Load the Identity Matrix to reset our drawing locations
	//gluLookAt(0, 0, -10, 0, 0, -1, 0, 1, 0);
	glTranslatef(xLocation, 0.0f, 0.0f); // Push everything 5 units back into the scene, otherwise we won't see the primitive

	glTranslatef(0.0f, yLocation, 0.0f); // Translate our object along the y axis
	glTranslatef(0.0f, 0.0f, zLocation);
	//glRotatef(yRotationAngle, 0.0f, 1.0f, 0.0f); // Rotate our object around the y axis
	glRotatef(xRotationAngle, 1.0f, 0.0f, 0.0f);
	glRotatef(yRotationAngle, 0.0f, 1.0f, 0.0f);

	//glColor3f(red,green,blue);
	glColor3f( 1.0f, 1.0f, 0.0f );
	glutSolidTeapot(20.0f);

	// Draw X/Y/Z Lines
	glPushMatrix();
	glColor3f( 1.0f, 1.0f, 1.0f );
	glBegin( GL_LINES );
	glVertex3f( -100.0f, 0.0f, 0.0f );
	glVertex3f( 100.0f, 0.0f, 0.0f );
	glVertex3f( 0.0f, -100.0f, 0.0f );
	glVertex3f( 0.0f, 100.0f, 0.0f );
	glVertex3f( 0.0f, 0.0f, -100.0f );
	glVertex3f( 0.0f, 0.0f, 100.0f );            
	glEnd();
	glPopMatrix();

	//glutSolidTeapot( 20.0f );
	calcFPS();
	//drawFPS();
	//renderPrimitive(); // Render the primitive
	//drawSky();
	glutSwapBuffers(); // Flush the OpenGL buffers to the window

}
void reshape(int w, int h){
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if(h == 0)
		h = 1;
	float ratio = 1.0* w / h;

	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);

	// Reset Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);
	// Set the correct perspective.
	gluPerspective(45,ratio,1,1000);

	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);
}


//  define the window position on screen
int window_x;
int window_y;
void centerOnScreen ()
{
	window_x = (glutGet (GLUT_SCREEN_WIDTH) - width)/2;
	window_y = (glutGet (GLUT_SCREEN_HEIGHT) - height)/2;
}

void setupWindow(int argc, char* args[]){

	glfwDisable(GLFW_MOUSE_CURSOR); // Hide the mouse cursor
	//GLUT Window Initialization
	glutInit(&(argc),args);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DEPTH|GLUT_DOUBLE);
	
	glutInitWindowSize(width,height);
	centerOnScreen ();
	glutInitWindowPosition(window_x ,window_y);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glutCreateWindow("skyGesture Pre-Alpha Build v0.001a");

	//register callback
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(display);
	glutKeyboardFunc(pressNormalKeys);
	glutKeyboardUpFunc(releaseNormalKeys);
	glutSpecialFunc(pressSpecialKeys);
	glutSpecialUpFunc(releaseSpecialKeys);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotion);
	
	// OpenGL init
	//glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	//initialize loop
	for(int i = 0; i < 256; i++) 
		keyStates[i] = false; 
	for(int i = 0; i < 246; i++)
		keySpecialStates[i] = false; 

	//enter GLUT event processing cycle
	glutMainLoop();
}








int main( int argc, char* args[] )
{

	string input, buffer;
	ofstream myfile;
	cout << "Welcome to skyGesture Application.\nThis program requires a .cvs file.\nPlease note that it has to be in the same directory as this executable.\n"<<endl;
	/*
	cout << "Enter file name: "<< endl;
	cin >> input;
	myfile.open (input);
	if (myfile.is_open()){
	while ( myfile.good() ){
	getline(myfile,buffer);
	cout << buffer << endl;
	}
	myfile.close();
	}

	else cout << "Unable to open file"; 
	*/
	cout << "Type in 'start' to start "<< endl;
	do{
		cout << ">>";
		getline(cin, input);
		if(input.compare("start")==0){
			setupWindow(argc,args);
		}
	} while(input.compare("quit") != 0);

	return 0;    
}