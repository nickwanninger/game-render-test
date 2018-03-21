#ifndef __context__
#define __context__

#include <SDL2/SDL.h>

#define SCALE 3
#define WIDTH 270
#define HEIGHT 190
// #define WIDTH 160
// #define HEIGHT 120

typedef struct {
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* texture;
	int w;
	int h;
	float scale;
} context_t;

context_t* contextinit();

#endif