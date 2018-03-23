#ifndef __GL_LIB__
#define __GL_LIB__


#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <stdbool.h>


#ifdef __APPLE__
	// Apple Specific compilation things here.
	#include <openGL/GL.h>
	#include <openGL/GLU.h>
#endif

#ifdef __LINUX__
	#include <GL/gl.h>
	#include <GL/glu.h>
#endif

#ifdef __WINDOWS__
	// Windows Compilation specific things here.
	#include <gl/gl.h>
	#include <gl/glu.h>
	#include <gl/wglext.h>
#endif


bool initGL();

#endif
