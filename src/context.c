#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "context.h"
#include "util.h"


pthread_mutex_t resizemutex = PTHREAD_MUTEX_INITIALIZER;

int
contextresize(context_t* context, int width, int height, float scale) {

	pthread_mutex_lock(&resizemutex);
	context->width = width;
	context->height = height;
	context->scale = scale;
	SDL_SetWindowSize(context->window, (int)(width * scale), (int)(height * scale));
	SDL_DestroyTexture(context->texture);
	// free(context->texture);
	context->texture = SDL_CreateTexture(context->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);
	if (context->texture == NULL) {
		fprintf(stderr, "Fatal: memory error when resizing window\n");
		exit(-1);
	}
	pthread_mutex_unlock(&resizemutex);
	return 1;
}

context_t*
contextinit(int width, int height, float scale) {
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* window = SDL_CreateWindow("render thing", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, (int) (width * scale), (int) (height * scale), SDL_WINDOW_SHOWN);
	// SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);
	context_t* ctx = xmalloc(sizeof(context_t));
	ctx->window = window;
	ctx->renderer = renderer;
	ctx->texture = texture;
	ctx->width = width;
	ctx->height = height;
	ctx->scale = scale;
	return ctx;
}