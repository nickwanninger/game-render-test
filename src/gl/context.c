#include "context.h"
#include "window.h"
#include <stdio.h>


extern int window_width;
extern int window_height;


bool
contextinit() {
	context = SDL_GL_CreateContext(window);

	if (context == NULL) {
		printf( "OpenGL context could not be created! SDL Error: %s\n", SDL_GetError() );
		return false;
	}

	if(SDL_GL_SetSwapInterval( 1 ) < 0) {
		printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	return true;
}