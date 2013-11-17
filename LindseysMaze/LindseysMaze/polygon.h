#pragma once
#include <glut.h>
#include <glu.h>

class polygon {
	int type;
	int a;
	int b;
	int c;
	int d;
public:
	polygon(void){
		a = 0;
		b =0;
		c= 0;
		d = 1;
	}

	polygon(int ain, int bin, int cin){
		type = 3;
		a = ain;
		b = bin;
		c = cin;
		d = 1;
	}
	polygon(int ain, int bin, int cin, int din){
		type = 4;
		a = ain;
		b= bin;
		c = cin;
		d = din;
	}
	int geta (void){
		return a;
	}
	int getb (void){
		return b;
	}
	int getc (void){
		return c;
	}
	int getd (void){
		return d;
	}
	int getType (void){
		return type;
	}
	int* getPointer(void) {
		return &a;
	}
};