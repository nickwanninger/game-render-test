#ifndef __context__
#define __context__

#include <SDL2/SDL.h>

typedef struct {
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* texture;
	int w;
	int h;
	float scale;
} context_t;

#endif