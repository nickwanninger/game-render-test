#include <stdio.h>
#include <SDL2/SDL.h>
#include <math.h>

#include <stdint.h>

#include "level.h"
#include "context.h"
#include "player.h"
#include "point.h"
#include "types.h"
#include "art.h"
#include "render.h"
#include "input.h"


long ticks = 0;
long frames = 0;

const float scale = 2.0f;
const int width = (int) (1280 / 2.0);
const int height = (int) (900 / 2.0);


const char* map[] = {
	"11111",
	"10001",
	"10011",
	"10001",
	"11111"
};


context_t
init(void) {
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* window = SDL_CreateWindow("render thing", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, (int) (width * scale), (int) (height * scale), SDL_WINDOW_SHOWN);
	// SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);
	context_t ctx = {window, renderer, texture, width, height, scale};
	return ctx;
}


static int done() {
	SDL_Event event;
	SDL_PollEvent(&event);
	return event.type == SDL_QUIT || event.key.keysym.sym == SDLK_ESCAPE;
}


int
main(int argc, char** argv) {
	// Initialize the graphics
	ginit();

	level_t level = lopen("assets/level.bmp");

	player_t player = {
		{level.pspawn.x, level.pspawn.y, 0}, // Position
		{0, 0}, // Velocity
		{0, 0}, // Acceleration
		0.0f, // Rotation
		0.0f, // Rotation Acceleration
		0.1 / 6.0, // Walk Speed
	};

	context_t context = init();
	while(!done()) {
		handleinput(&player, &level);
		render(context, &player, level);
	}
	return 0;
}
