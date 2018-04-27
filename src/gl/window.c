#include "window.h"
#include <stdio.h>

int window_width = 1280;
int window_height = 720;



bool
windowinit() {
	
	// SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	// SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	// SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	// SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	// window = SDL_CreateWindow("OpenGL", 100, 100, 800, 600, SDL_WINDOW_OPENGL);
	
	window = SDL_CreateWindow("Game Engine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_width, window_height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);


	if (window == NULL) {
		printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	return true;
}
