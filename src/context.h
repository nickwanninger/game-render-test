#ifndef __context__
#define __context__

#include <SDL2/SDL.h>

// #define SCALE 4
// #define WIDTH 270
// #define HEIGHT 190

typedef struct {
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* texture;
	int width;
	int height;
	float scale;
} context_t;


int contextresize(context_t* context, int width, int height, float scale);
context_t* contextinit(int width, int height, float scale);

#endif