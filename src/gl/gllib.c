#include "gllib.h"
#include "window.h"
#include "context.h"
#include <stdio.h>
#include <stdbool.h>

extern int window_width;
extern int window_height;


bool
initGL() {
	GLenum error = GL_NO_ERROR;
	
	glMatrixMode(GL_PROJECTION);
	
	glLoadIdentity();
	
	error = glGetError();
	if (error != GL_NO_ERROR) return false;
	

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	// glEnable(GL_NEAREST);
	
	error = glGetError();
	if( error != GL_NO_ERROR ) return false;

	return true;
}
