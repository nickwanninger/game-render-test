#include "gl/gllib.h"
#include "gl/window.h"
#include "gl/context.h"

#include <math.h>
#include <getopt.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>

#include "level.h"
#include "camera.h"
#include "point.h"
#include "types.h"
#include "bitmap.h"
#include "input.h"
#include "shader.h"
#include "game.h"



#define PI 3.141592653589793238

#define MODE_PLAY 0
#define MODE_EDIT 1


static struct option const long_options[] = {
	{"edit", optional_argument, NULL, 'e'},
	{NULL, 0, NULL, 0}
};


//window dimension constants
extern int window_width;
extern int window_height;








int
main(int argc, char** argv) {

	


	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "SDL could not initialize. Error: %s", SDL_GetError());
		return false;
	}

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






