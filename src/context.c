#include <SDL2/SDL.h>
#include <stdlib.h>
#include "context.h"





context_t*
contextinit() {
	
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* window = SDL_CreateWindow("render thing", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, (int) (WIDTH * SCALE), (int) (HEIGHT * SCALE), SDL_WINDOW_SHOWN);
	// SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
	context_t* ctx = malloc(sizeof(context_t));
	ctx->window = window;
	ctx->renderer = renderer;
	ctx->texture = texture;
	ctx->w = WIDTH;
	ctx->h = HEIGHT;
	ctx->scale = SCALE;
	return ctx;
}