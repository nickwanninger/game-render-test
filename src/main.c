#include <stdio.h>
#include <SDL2/SDL.h>
#include <math.h>
#include <getopt.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>


#include "level.h"
#include "context.h"
#include "camera.h"
#include "point.h"
#include "types.h"
#include "art.h"
#include "input.h"

#include "game.h"


#define MODE_PLAY 0
#define MODE_EDIT 1


// #define __SINGLE_THREADED__


// const float scale = 4;
// const int width = 280;
// const int height = 190;
// const float scale = 3;
// const int width = 350;
// const int height = 260;


// pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


camera_t camera = {};
context_t context = {};
level_t level = {};


const char* map[] = {
	"11111",
	"10001",
	"10011",
	"10001",
	"11111"
};



static struct option const long_options[] = {
	{"edit", optional_argument, NULL, 'e'},
	{NULL, 0, NULL, 0}
};


#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"


int
main(int argc, char** argv) {

	ginit();
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

	game_t* game = gameinit(filename);

	if (mode == MODE_PLAY) {
		printf("Playing level file '%s'\n", filename);
		gamestart(game);
	} else if (mode == MODE_EDIT) {
		printf("Editing level file '%s'\n", filename);
	}

	return 0;
}
