#include <stdio.h>
#include <SDL2/SDL.h>
#include <math.h>
#include <getopt.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>

#include "level.h"
#include "context.h"
#include "player.h"
#include "point.h"
#include "types.h"
#include "art.h"
#include "render.h"
#include "input.h"


#define MODE_PLAY 0
#define MODE_EDIT 1


// #define __SINGLE_THREADED__


long ticks = 0;
long frames = 0;
int fps = 0;

// const float scale = 4;
// const int width = 280;
// const int height = 190;
const float scale = 3;
const int width = 350;
const int height = 260;

int microsecondsinsecond = 1000000;
float microsecondsintick = 1000000 / 60.0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


player_t player = {};
context_t context = {};
level_t level = {};


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


unsigned long
utime() {
	struct timeval tv;
	// gettimeofday(&tv,NULL);
	return tv.tv_usec;
}

void*
renderThreadLoop(void *arg) {
	unsigned long t0, t1;
	int currentus = 0;
	int framesthissecond = 0;
	while (1) {
		
		t0 = utime();
		render(context, &player, level);
		frames++;
		t1 = utime();
		int us = microsecondsintick - (t1 - t0);
		currentus += us;
		framesthissecond += 1;
		if (currentus >= 1000) {
			fps = framesthissecond;
			framesthissecond = 0;
			currentus -= microsecondsinsecond;
		}
		// usleep(rand() % 10000);
		
	}
	pthread_exit(0);
	return 0;
}


int playlevel(char* filename) {
	// Initialize the graphics
	ginit();
	// Read the level data from the bitmap.
	level = lopen(filename);
	player = (player_t) {
		{level.pspawn.x, level.pspawn.y, 0}, // Position
		{0, 0}, // Velocity
		{0, 0}, // Acceleration
		0.0f, // Rotation
		0.0f, // Rotation Acceleration
		0.1 / 3.5, // Walk Speed
	};
	context = init();

	#ifndef __SINGLE_THREADED__
	// Create the render thread.
	pthread_t renderthread;
	pthread_create(&renderthread, NULL, renderThreadLoop, 0);
	#endif

	// Loop and handle user input and ticking till the user wants to quit.
	while(!done()) {
		#ifndef __SINGLE_THREADED__
		pthread_mutex_lock(&mutex);
		#endif
		#ifdef __SINGLE_THREADED__
		render(context, &player, level);
		frames++;
		#endif
		handleinput(&player, &level);
		usleep(microsecondsintick);

		#ifndef __SINGLE_THREADED__
		pthread_mutex_unlock(&mutex);
		#endif
		
	}
	exit(1);
}


static struct option const long_options[] = {
	{"edit", optional_argument, NULL, 'e'},
	{NULL, 0, NULL, 0}
};


int
main(int argc, char** argv) {

	char* programname = argv[0];
	int mode = MODE_PLAY;

	// Start parsing some args
	char c = 0;
	while ((c = getopt_long(argc, argv, "e", long_options, NULL)) != -1) {
		switch (c) {
			case 'e': // verbose mode
				printf("Edit Mode\n");
				mode = MODE_EDIT;
				break;
		}
	}

	// If there arent any args left (the file name)
	if (optind >= argc) {
		printf("Usage: %s [OPTION]... <level file>\n", programname);
		return EXIT_FAILURE;
	}
	
	char* filename = argv[optind++];

	if (mode == MODE_PLAY) {
		printf("Playing level file '%s'\n", filename);
		playlevel(filename);
	} else if (mode == MODE_EDIT) {
		printf("Editing level file '%s'\n", filename);
	}
	
	// // Run over each file in the arguments
	// while (optind < argc) {
		
	// 	// Check for file read errors
	// 	if (!(source->file = fopen(source->name, "r"))) {
	// 		fclose(source->file);
	// 		printf("File error: %s\n", strerror(errno));
	// 		return EXIT_FAILURE;
	// 	}

	// 	parsefile(source);
	// 	fclose(source->file);
	// 	free(source);
	// }

	return 0;
}
