#pragma once
#include <stdio.h>
#include <sys/stat.h>

#include <stdlib.h>
#include <windows.h>
#include <objidl.h>
/// Windows libraries needed for images
#include <GdiPlus.h>
#include <GdiPlusGraphics.h>
using namespace Gdiplus;
#pragma comment(lib, "GdiPlus.lib")

#include <glew.h>
//#include <freeglut.h>
#include <glut.h>
#include <iostream>
#include <istream>
#include <string.h>
#include <fstream>
#include <string>
#include <vector>

#include "vertex3.h"
#include "polygon.h"
#include "shader.h"
#include "ModelView.h"
using namespace std;
#include <math.h>

/* ascii codes for various special keys */
#define ESCAPE 27

/**********************************************************************
 * Configuration
 **********************************************************************/

#define INITIAL_WIDTH    640
#define INITIAL_HEIGHT   480
#define WINDOW_NAME     "Dirt Maze!"

#define PI				 3.1415926535 /* used for calculating angles */



 /**********************************************************************
 * Globals
 **********************************************************************/
float posX; //will use for glulookat and collision detection
float posZ; //will use for glulookat and collision detection
float prevX;
float prevZ;
float dirX;
float dirY;
float dirZ;
float angle;
float step = 0.2;
float X, Y, Z;//for translate updates
GLsizei window_width;
GLsizei window_height;
GLuint m_wall_texture_id;
GLuint m_wall_texture_id2;

bool win;
int gameover;

//Objects
ModelView tunnel("resources/tunnels2.txt", false);
ModelView knight("resources/knight.txt", true);
ModelView bishop("resources/bishop.txt", true);

GLuint shader = 0;

// This function does all the work and there is also a little
// startup/shutdown code for GDI library in the main function
// (and that is needed).
GLuint png_to_texture(const char* file_name)
{
	GLuint   tex_id;  

	// Convert the image filename to unicode for the Bitmap constructor function:
	int lenA = lstrlenA(file_name);
	int lenW;
	BSTR unicodestr;

	lenW = ::MultiByteToWideChar(CP_ACP, 0, file_name, lenA, 0, 0);
	if (lenW > 0)
	{
		// Check whether conversion was successful
		unicodestr = ::SysAllocStringLen(0, lenW);
		::MultiByteToWideChar(CP_ACP, 0, file_name, lenA, unicodestr, lenW);
	}
	else
	{
		//unicode error..
		return 0;
	}

	// Read in the image:
	Gdiplus::Bitmap img(unicodestr);
	
	// when done, free the BSTR
	::SysFreeString(unicodestr);

	/// Get all pixels from the image:
    int w = img.GetWidth();
    int h = img.GetHeight();   
    unsigned char* bits = (unsigned char*)malloc(w*h*3*2);  

    unsigned char* addr = bits;
    for (int y=0; y<h; ++y) {
        for (int x=0; x<w; ++x) {
            addr = bits + y*w*3 + x*3;

			Gdiplus::Color color;
            img.GetPixel(x,y, &color);
            unsigned char r,g,b;
            r = color.GetR();
            g = color.GetG(); 
            b = color.GetB();           

            *addr = r; ++addr;
            *addr = g; ++addr;
            *addr = b;
        }
    }

	// Create a texture from the image:
    glGenTextures (1, &tex_id);
    //glBindTexture(GL_TEXTURE_2D, tex_id);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexImage2D(GL_TEXTURE_2D, 0, 
        GL_RGB, 
        w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, bits);

    return tex_id;
}

void init()
{
	// init GLEW
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
	  /* Problem: glewInit failed, something is seriously wrong. */
	  fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	m_wall_texture_id = png_to_texture("resources\\brick_color_map.png");
	m_wall_texture_id2 = png_to_texture("resources\\topsoil_screened_lg.png");
	glClearColor(1.0, 1.0, 1.0, 1.0);

	//glMatrixMode(GL_MODELVIEW);

	//glMatrixMode(GL_PROJECTION);
	//gluPerspective(45, (GLfloat) window_width / (GLfloat) window_height, 1, 100);	

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthMask(GL_LEQUAL);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

    glEnable(GL_LIGHTING); //Enable lighting
    glEnable(GL_LIGHT0); //Enable light #0
    glEnable(GL_LIGHT1); //Enable light #1
    glEnable(GL_NORMALIZE); //Have OpenGL automatically normalize our normals
    glShadeModel(GL_SMOOTH); //Enable smooth shading

	//glEnable(GL_FOG);  //Intended for extra credit!!
	shader = createShader( "shader/phong" );
}

void resize_scene(GLsizei width, GLsizei height)
{
    glViewport(0, 0, width, height);  /* reset the current viewport and 
                                   
    * perspective transformation */
	window_width  = width;
    window_height = height;

    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70, (GLfloat) window_width / (GLfloat) window_height, 1, 200);
    //glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
//	glOrtho(-2,2,-2,2,-2,2);
    /* for the purposes of this assignment, we are making the world
     * coordinate system have a 1-1 correspondence with screen space
     * (in pixels).  The origin maps to the lower-left corner of the screen.*/
    
    //glutPostRedisplay();

}

void draw_room(void)
{
	printf("In draw_room function");
	glEnable( GL_TEXTURE_2D );
	//glBindTexture( GL_TEXTURE_2D, m_wall_texture_id );

    glBegin( GL_QUADS );
		// Floor
        glNormal3f( 0.0f, 1.0f, 0.0f );
		glTexCoord2f( 0.0f, 0.0f );
        glVertex3f( -10.0f, -10.0f, -10.0f );
		glTexCoord2f( 1.0f, 0.0f );
        glVertex3f( -10.0f, -10.0f, 10.0f );
		glTexCoord2f( 1.0f, 1.0f );
        glVertex3f( 10.0f, -10.0f, 10.0f );
		glTexCoord2f( 0.0f, 1.0f );
        glVertex3f( 10.0f, -10.0f, -10.0f );
	glEnd();

	//glBindTexture(GL_TEXTURE_2D, m_wall_texture_id2);
    glBegin( GL_QUADS );
		// Ceiling
		glColor3f( 0.0, 1.0, 0.0 );
        glNormal3f( 0.0f, -1.0f, 0.0f );
		glTexCoord2f( 0.0f, 0.0f );
        glVertex3f( -10.0f, 10.0f, 10.0f );
		glTexCoord2f( 1.0f, 0.0f );
        glVertex3f( -10.0f, 10.0f, -10.0f );
		glTexCoord2f( 1.0f, 1.0f );
        glVertex3f( 10.0f, 10.0f, -10.0f );
		glTexCoord2f( 0.0f, 1.0f );
        glVertex3f( 10.0f, 10.0f, 10.0f );
	glEnd();

    glBegin( GL_QUADS );
		// Front wall
		glColor3f( 0.0, 0.0, 1.0 );
        glNormal3f( 0.0f, 0.0f, 1.0f );
		glTexCoord2f( 0.0f, 0.0f );
        glVertex3f( -10.0f, 10.0f, -10.0f );
		glTexCoord2f( 1.0f, 0.0f );
        glVertex3f( -10.0f, -10.0f, -10.0f );
		glTexCoord2f( 1.0f, 1.0f );
        glVertex3f( 10.0f, -10.0f, -10.0f );
		glTexCoord2f( 0.0f, 1.0f );
        glVertex3f( 10.0f, 10.0f, -10.0f );

        // Back wall
		glColor3f( 1.0, 1.0, 0.0 );
        glNormal3f( 0.0f, 0.0f, -1.0f );
		glTexCoord2f( 0.0f, 0.0f );
        glVertex3f( 10.0f, 10.0f, 10.0f );
		glTexCoord2f( 1.0f, 0.0f );
        glVertex3f( 10.0f, -10.0f, 10.0f );
		glTexCoord2f( 1.0f, 1.0f );
        glVertex3f( -10.0f, -10.0f, 10.0f );
		glTexCoord2f( 0.0f, 1.0f );
        glVertex3f( -10.0f, 10.0f, 10.0f );

        // Left wall
		glColor3f( 0.0, 1.0, 1.0 );
        glNormal3f( 1.0f, 0.0f, 0.0f );
		glTexCoord2f( 0.0f, 0.0f );
        glVertex3f( -10.0f, 10.0f, 10.0f );
		glTexCoord2f( 1.0f, 0.0f );
        glVertex3f( -10.0f, -10.0f, 10.0f );
		glTexCoord2f( 1.0f, 1.0f );
        glVertex3f( -10.0f, -10.0f, -10.0f );
		glTexCoord2f( 0.0f, 1.0f );
        glVertex3f( -10.0f, 10.0f, -10.0f );

        // Right wall
		glColor3f( 1.0, 0.0, 1.0 );
        glNormal3f( -1.0f, 0.0f, 0.0f );
		glTexCoord2f( 0.0f, 0.0f );
        glVertex3f( 10.0f, 10.0f, -10.0f );
		glTexCoord2f( 1.0f, 0.0f );
        glVertex3f( 10.0f, -10.0f, -10.0f );
		glTexCoord2f( 1.0f, 1.0f );
        glVertex3f( 10.0f, -10.0f, 10.0f );
		glTexCoord2f( 0.0f, 1.0f );
        glVertex3f( 10.0f, 10.0f, 10.0f );
	glEnd();

	glDisable( GL_TEXTURE_2D );
}

void draw_scene(void)
{

	//glUniform3f( glGetUniformLocation( shader, "light" ), 0, 200, 200 );

    /* clear the screen and the depth buffer */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
    /* reset modelview matrix */
    glLoadIdentity();

	GLfloat lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat lightAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f};
	GLfloat lightPosition[] = { 0.0, 200.0, 200.0, 1.0f};

    gluLookAt(posX, 0.0, posZ, 
			  posX + dirX, 0.0, posZ + dirZ,
		      0.0, 1.0, 0.0);
	//gluLookAt(posX, 0, posZ, posX + sin(angle), 0, posZ - cos(angle), 0, 1, 0);

	glLightfv( GL_LIGHT0, GL_POSITION, lightPosition );
	glLightfv( GL_LIGHT0, GL_AMBIENT, lightAmbient );
	glLightfv( GL_LIGHT0, GL_DIFFUSE, lightDiffuse );
	
	glColor3f(1.0f, 1.0f, 1.0f);

	glPushMatrix();
	glTranslatef(X, Y, Z);
   //draw_room();

	tunnel.draw_object(1.0, 1.0, 1.0, 0.0, 0.0, 0.0, m_wall_texture_id, m_wall_texture_id2);


   //Here is where we define and draw objects
	glUseProgram( shader );
	knight.draw_object(1.0, 0.0, 0.0, 0.0, -2.5, 2.0, 0, 0);
	bishop.draw_object(1.0, 0.0, 0.0, 20.0, -2.5, 40.0, 0, 0);
	glUseProgram( 0 );

  // glFinish();
   glPopMatrix();
    /* since this is double buffered, swap the
     * buffers to display what just got drawn */
   glutSwapBuffers();

}

bool collision(float prevX, float prevZ, float X, float Z)
{
	if(((prevX < 2.5) && (prevX > -2.5)) && ((prevZ < 15.0) && (prevZ > 0))) //Zone 1
	{
		if((X > 2.3) || (Z > 14.7) ||  (Z < .3))
			return false;
		if((X < -2.3) && (Z < 10.0))
				return false;
		if (Z < 2.5){
			printf("\nYou win!!");
		}
	}
	else if (((prevX < -2.5) && (prevX > -7.5)) && ((prevZ < 15.0) && (prevZ > 10.0))) //Zone 2
	{
		if ((Z > 14.8) || (Z < 10.2))
			return false;
	}
	else if (((prevX < -7.5) && (prevX > -12.5)) && ((prevZ < 30.0) && (prevZ > 10.0))){ //Zone 3
		if ((Z > 29.8) || (Z < 10.2))
			return false;
		if ((X > -7.3) &&  ((Z > 15.0) && (Z < 25.0)))
			return false;
		if(X < -12.3)
			return false;
	}
	else if (((prevX < 7.5) && (prevX > -7.5)) && ((prevZ < 30.0) && (prevZ > 25.0))){ //Zone 4
		if(Z < 25.2)
			return false;
		if ((Z > 29.8) && !((X > -2.5) && (X < 2.5)))
			return false;
	}
	else if(((prevX < 12.5) && (prevX > 7.5)) && ((prevZ < 30.0) && (prevZ > 25.0))){ //Zone 5
		if((X > 11.8) || (Z > 29.8))
			return false;
	}
	else if(((prevX < 12.5) && (prevX > 7.5)) && ((prevZ < 25.0) && (prevZ > 20.0))){ // Zone 6
		if(Z < 20.2)
			return false;
		printf("\nGAME OVER, please try again.\n");
		posX = 0.0;
		posZ = 40.0;
		gameover += 1;
	}
	else if(((prevX < 2.5) && (prevX > -2.5)) && ((prevZ < 40.0) && (prevZ > 30.0))){
		if(Z > 39.8)
			win = true;
		if((X > 2.3) || (X < -2.3))
			return false;
	}
	return true;
}

void key_press( unsigned char key, int x, int y) 
{
	 switch ( key )
	{
	 case 'w' : //move camera in, move towards
		 prevX = posX;
		 prevZ = posZ;
		if(collision(prevX, prevZ, (posX+ (dirX *step)), (posZ + (dirZ *step)))){
			posZ += dirZ * step;
			posX += dirX * step;
		 }
		 else
			 cout <<  "There's a wall there!" << endl;
		 break;
	 case 'z' : //move camera away
		 prevX = posX;
		 prevZ = posZ;
		 if(collision(prevX, prevZ, (posX- (dirX *step)), (posZ - (dirZ *step)))){
			posZ += dirZ * step;
			posX += dirX * step;
		 }
		 else
			 cout << "There's a wall there!" << endl;
		 posZ -= dirZ * step;
		 posX -= dirX * step;
		 break;
	 case 'a' : //move camera left
		 //posX += dirZ * .1;
		 //posZ -= dirX * .1;
		 angle -= 0.05f;
		 dirX = sin(angle);
		 dirZ = -cos(angle);
		 //angle += (PI / 180.0f) * .1;
		 break;
	 case 's' : //move camera right
		 //posX += dirZ * .1;
		 //posZ += dirX * .1;
		// angle -= (PI / 180.0f) * .1;
		 angle += 0.05f;
		 dirX = sin(angle);
		 dirZ = -cos(angle);
		 break;
	 }
    glutPostRedisplay();
}

/**********************************************************************
 * this function is called whenever a mouse button is pressed and moved
 **********************************************************************/

void handle_mouse_motion(int x, int y)
{
}

/**********************************************************************
 * this function is called whenever a mouse button is pressed or released
 **********************************************************************/
void handle_mouse_click(int btn,int state,int x,int y)
{
}

void special_key(int key, int x, int y) { 

// The keys below are using the gluLookAt() function for navigation
// Check which key is pressed

switch(key) {
    case GLUT_KEY_LEFT : // Rotate on x axis
    X -= 0.1f;
    break;
    case GLUT_KEY_RIGHT : // Rotate on x axis (opposite)
    X += 0.1f;
    break;
    case GLUT_KEY_UP : // Rotate on y axis 
    Y += 0.1f;
    break;
    case GLUT_KEY_DOWN : // Rotate on y axis (opposite)
    Y -= 0.1f;
    break; 
    case GLUT_KEY_PAGE_UP: // Rotate on z axis
    Z -= 0.1f;
    break;
    case GLUT_KEY_PAGE_DOWN:// Rotate on z axis (opposite)
    Z += 0.1f;
    break;
}
    glutPostRedisplay(); // Redraw the scene
}


int main(int argc, char * argv[]) 
{  
	//Set initial Global Variables
	posX = 0.0;
	posZ = 40.0;
	dirX = 0.0;
	dirY = -1.0;
	dirZ = 0.0;
	angle = 0.0;
	X = 0.0;
	Z = 0.0;
	Y = 0.0;
	//Initialize game settings
	gameover = 0;
	win = false;


  	/* Initialize GLUT */
    glutInit(&argc, argv);  
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);  
    glutInitWindowSize(INITIAL_WIDTH, INITIAL_HEIGHT);  
    //glutInitWindowPosition(INITIAL_X_POS, INITIAL_Y_POS);  
    glutCreateWindow(WINDOW_NAME);  

	// Initialize GDI+. (for image loading)
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    /* Register callback functions */
	glutDisplayFunc(draw_scene);     
    glutReshapeFunc(resize_scene);       //Initialize the viewport when the window size changes.
    glutKeyboardFunc(key_press);         //handle when the key is pressed
    //glutMouseFunc(handle_mouse_click);   //check the Mouse Button(Left, Right and Center) status(Up or Down)
	//glutSpecialFunc(special_key);        //Special Keyboard Key fuction(For Arrow button and F1 to F10 button)

	// Load anything (images, models) needed while running
	init();
	printf("Find the knight and you win, find the bishop and you lose!\n\n Press s to start\n");

    
    /* Enter event processing loop */
    glutMainLoop();  


	/// Added for image loading
	GdiplusShutdown(gdiplusToken);

    return 1;
}