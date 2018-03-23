#include "gl/window.h"

#include "input.h"
#include "level.h"
#include "camera.h"
#include "game.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "script.h"
#include "console.h"

#define __HANDLE_COLLISIONS__


extern int window_width;
extern int window_height;


#define SIGN(x) (x >= 0 ? 1 : -1)
int consoletogglecooldown = 0;

const uint8_t* keystate;

void
inputEventWatcher(game_t* game) {
	SDL_StartTextInput();
	// SDL_SetRelativeMouseMode(SDL_TRUE);
	SDL_Event event;
	char entered;
	while (1) {
		game->keystate = (uint8_t *) SDL_GetKeyboardState(NULL);
		while (SDL_WaitEvent(&event)) {


			if (event.type == SDL_WINDOWEVENT) {
				switch (event.window.event) {
					case SDL_WINDOWEVENT_RESIZED:
						printf("resize\n");
						window_width = event.window.data1;
						window_height = event.window.data2;
						
						break;
				}
			}


			
			switch (event.type) {
				// case SDL_MOUSEMOTION:
				// 	game->camera->roty += event.motion.xrel / 180.0;
				// 	game->camera->rotx += event.motion.yrel / (180.0 * 2);
				// 	break;
				case SDL_MOUSEWHEEL:
					// printf("Scroll: %d\n", event.wheel.y);
					break;
				case SDL_QUIT:
					printf("Quitting...\n");
					exit(0);
				case SDL_TEXTINPUT:
					entered = event.text.text[0];
					if (entered == '`') {
						game->console->active = !game->console->active;
						break;
					}
					if (game->console->active) {
						consoleinputaddchar(game->console, event.text.text[0]);
					}
					break;
			}
		}
	}
}

void
handleinput(game_t* game) {

	if (game->keystate[SDL_SCANCODE_ESCAPE]) exit(0);


	
	// double movespeed = camera->walkSpeed;

	// double xm = camera->vx;
	// double ym = camera->vy;

	// float rotspeed = 0.04f;

// 	if (!game->console->active) {
// 		if(game->keystate[SDL_SCANCODE_LSHIFT]) movespeed *= 2;

// 		// Rotation
// 		if(game->keystate[SDL_SCANCODE_LEFT]) camera->rota -= rotspeed;
// 		if(game->keystate[SDL_SCANCODE_RIGHT]) camera->rota += rotspeed;

// 		if(game->keystate[SDL_SCANCODE_W]) ym--;
// 		if(game->keystate[SDL_SCANCODE_S]) ym++;
// 		if(game->keystate[SDL_SCANCODE_D]) xm--;
// 		if(game->keystate[SDL_SCANCODE_A]) xm++;

// 	}

	
	
// 	double dd = xm
// 	 * xm + ym * ym;
// 	if (dd > 0) dd = sqrt(dd);
// 	else dd = 1;
// 	xm /= dd;
// 	ym /= dd;

// 	camera->turnBob *= 0.8;
// 	camera->turnBob += camera->rota;

// 	camera->bob *= 0.6;
// 	camera->bob += sqrt(xm * xm + ym * ym);
// 	camera->bobPhase += sqrt(xm * xm + ym * ym);

// 	camera->ax -= (xm * cos(camera->rot) + ym * sin(camera->rot)) * movespeed;
// 	camera->ay -= (ym * cos(camera->rot) - xm * sin(camera->rot)) * movespeed;

// 	#ifdef __HANDLE_COLLISIONS__
// 	int tx = floor(camera->x);
// 	int ty = floor(camera->y);
// 	double r = 0.1;
// 	int xs = (camera->ax > 0) - (camera->ax < 0);
// 	int ys = (camera->ay > 0) - (camera->ay < 0);
// 	int newtx = floor(camera->x + camera->ax + r * xs);
// 	int newty = floor(camera->y + camera->ay + r * ys);
// 	uint32_t xb = getblock(*(game->level), newtx, ty);
// 	uint32_t yb = getblock(*(game->level), tx, newty);
// 	if ((xb != 0 && xb != 0x00ff00) || newtx < 0 || newtx > game->level->width) {
// 		camera->ax = 0;
// 	}
// 	if ((yb != 0 && yb != 0x00ff00) || newty < 0 || newty > game->level->height) {
// 		camera->ay = 0;
// 	}
// 	#endif /* __HANDLE_COLLISIONS__ */


// // printf("%f\n", camera->ax);

// 	camera->speed = sqrt(camera->ax * camera->ax + camera->ay * camera->ay);
// 	// Move the camera here.
// 	camera->x += camera->ax;
// 	camera->y += camera->ay;

	

// 	// Apply Friction
// 	camera->ax *= 0.6;
// 	camera->ay *= 0.6;
// 	camera->rot += camera->rota;
// 	camera->rota *= 0.4;

// 	// int rotsign = SIGN(camera->rot);
// 	if (camera->rot > M_PI * 2) camera->rot = 0;
// 	if (camera->rot < 0) camera->rot += M_PI * 2;

	
}
