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


// int frames = 0;

// float cx = 0.0f;
// float cz = 0.0f;
// float cy = 0.0f;
// float ay = 0.0f;
// float ax = 0.0f;
// void
// rendergl() {
// 	frames++;

// 	const uint8_t* keys = (uint8_t *) SDL_GetKeyboardState(NULL);


// 	float xm = 0.0f;
// 	float zm = 0.0f;

// 	if (keys[SDL_SCANCODE_D]) xm += 0.1f;
// 	if (keys[SDL_SCANCODE_A]) xm -= 0.1f;
// 	if (keys[SDL_SCANCODE_W]) zm += 0.1f;
// 	if (keys[SDL_SCANCODE_S]) zm -= 0.1f;

// 	float rotspeed = 2.0f;

// 	if (keys[SDL_SCANCODE_LEFT]) ay += rotspeed;
// 	if (keys[SDL_SCANCODE_RIGHT]) ay -= rotspeed;

// 	if (keys[SDL_SCANCODE_UP]) ax += rotspeed / 2.0;
// 	if (keys[SDL_SCANCODE_DOWN]) ax -= rotspeed / 2.0;


// 	double radians = ay * PI / 180;
// 	float rCos = cosf(radians);
// 	float rSin = sinf(radians);

// 	cx += (xm * rCos - zm * rSin);
// 	cz += (zm * rCos + xm * rSin);
	
// 	//Clear color buffer
// 	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

// 	glLoadIdentity();

// 	glEnable(GL_MULTISAMPLE);

// 	gluPerspective(45.0, window_width / (double) window_height, 0.01, 100.0);


// 	glRotatef(-ax, 1.0f, 0.0f, 0.0f);
// 	glRotatef(-ay, 0.0f, 1.0f, 0.0f);
// 	glTranslatef(-cx, -0.5f, cz);
	





// 	for (int i = 0; i < 30; i++) {
// 		glBegin(GL_QUADS);
// 			glColor3f(1.0f, 0.0f, 0.0f);
// 			glVertex3f(i + -0.5f, 0.0f, -0.5f);

// 			glColor3f(0.0f, 0.0f, 1.0f);
// 			glVertex3f(i + -0.5f, 0.0f, 0.5f);

// 			glColor3f(1.0f, 0.0f, 1.0f);
// 			glVertex3f(i + 0.5f, 0.0f, 0.5f);

// 			glColor3f(0.0f, 1.0f, 0.0f);
// 			glVertex3f(i + 0.5f, 0.0f, -0.5f);

// 		glEnd();
// 	}

// 	glBegin( GL_TRIANGLES );
// 		glColor3f(1.0f, 0.0f, 0.0f);
// 		glVertex3f(0.0f, 0.5f, 0.0f);

// 		glColor3f(0.0f, 0.0f, 1.0f);
// 		glVertex3f(-0.5f, -0.5f, 0.0f);

// 		glColor3f(0.0f, 1.0f, 0.0f);
// 		glVertex3f(0.5f, -0.5f, 0.0f);
// 	glEnd();
// }









int
main(int argc, char** argv) {

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "SDL could not initialize. Error: %s", SDL_GetError());
		return false;
	}

	// if(!init()) {
	// 	fprintf(stderr, "Failed to initialize!\n");
	// 	return 1;
	// }

	// SDL_Event e;
	// SDL_StartTextInput();
	// while(1) {
	// 	// Handle events on queue
	// 	while(SDL_PollEvent(&e) != 0){
	// 		// User requests quit
	// 		if (e.type == SDL_WINDOWEVENT) {
	// 			switch (e.window.event) {
	// 				case SDL_WINDOWEVENT_RESIZED:
	// 					window_width = e.window.data1;
	// 					window_height = e.window.data2;
	// 					glViewport(0, 0, window_width, window_height);
	// 					break;
	// 			}
	// 		}
	// 		if (e.type == SDL_QUIT) {
	// 			exit(1);
	// 			return 1;
				
	// 		}
	// 		// Handle keypress with current mouse position
	// 		else if (e.type == SDL_TEXTINPUT) {
	// 			int x = 0, y = 0;
	// 			SDL_GetMouseState( &x, &y );
	// 			// handleKeys( e.text.text[ 0 ], x, y );
	// 		}
	// 	}
	// 	// Render quad
	// 	rendergl();
	// 	// Update screen
	// 	glFinish();
	// 	SDL_GL_SwapWindow( window );
	// }

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






