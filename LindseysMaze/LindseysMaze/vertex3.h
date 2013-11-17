#pragma once
#include <glut.h>
#include <glu.h>

class vertex3
{
public:
	GLfloat x;
	GLfloat y;
	GLfloat z;
	vertex3(void){
		x = 0;
		y = 0;
		z = 0;
	}	
	vertex3 (GLfloat value){
		x = value;
		y = value;
		z = value;
	}
	vertex3(GLfloat xin, GLfloat yin, GLfloat zin){
		x = xin;
		y = yin;
		z = zin;
	}

	void zero(void){
		x = 0;
		y = 0;
		z = 0;
	}

	GLfloat getx (void){
		return x;
	}

	GLfloat gety (void){
		return y;
	}
	GLfloat getz (void){
		return z;
	}
	GLfloat* getPointer(void) {
		return &x;
	}

	vertex3 operator-(vertex3 vec){
		GLfloat X = x - vec.x;
		GLfloat Y = y - vec.y;
		GLfloat Z = z - vec.z;
		return vertex3(X, Y, Z);
	}

	vertex3 operator+(vertex3 vec){
		GLfloat X = x + vec.x;
		GLfloat Y = y + vec.y;
		GLfloat Z = z + vec.z;
		return vertex3(X, Y, Z);
	}

	vertex3 operator*( float scalar ){
		GLfloat X = x * scalar;
		GLfloat Y = y * scalar;
		GLfloat Z = z * scalar;
		return vertex3(X, Y, Z);
	}

	float dotProduct(vertex3 vec){
		return ((vec.x * x) + (vec.y * y) + (vec.z * z));
	}

	vertex3 crossProduct (vertex3 vec){
		GLfloat X = ((y * vec.z) - (vec.y * z));
		GLfloat Y = ((z * vec.x) - (vec.z * x));
		GLfloat Z = ((x * vec.y) - (vec.x * y));
		return vertex3(X, Y, Z);
	}
	
	void normalize(void){
		GLfloat mag = sqrt(x*x + y*y + z*z);
		if(mag < .000001) {
			x = 0;
			y = 0;
			z = 0;
			return;
		}
		x = x/mag;
		y = y/mag;
		z = z/mag;
	}

};

