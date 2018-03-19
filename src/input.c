#include "input.h"
#include "level.h"
#include "player.h"
#include <SDL2/SDL.h>
#include <math.h>

#define SIGN(x) (x >= 0 ? 1 : -1)

void
handleinput(player_t* player, level_t* level) {
	const uint8_t* key = SDL_GetKeyboardState(NULL);

	int tx = floor(player->pos.x);
	int ty = floor(player->pos.y);

	double movespeed = player->walkSpeed;


	// uint32_t onblock = getblock(*level, tx, ty);

	double xm = player->vel.x;
	double ym = player->vel.y;

	float rotspeed = 0.04f;
	if(key[SDL_SCANCODE_SPACE]) {
		rotspeed *= 0.05;
		movespeed *= 0.05;
		player->bob = 0;
		player->bobPhase = 0;
	}

	// Rotation
	if(key[SDL_SCANCODE_LEFT]) player->rota -= rotspeed;
	if(key[SDL_SCANCODE_RIGHT]) player->rota += rotspeed;

	if(key[SDL_SCANCODE_W]) ym--;
	if(key[SDL_SCANCODE_S]) ym++;
	if(key[SDL_SCANCODE_D]) xm--;
	if(key[SDL_SCANCODE_A]) xm++;

	if (key[SDL_SCANCODE_LSHIFT]) movespeed *= 2;

	double dd = xm
	 * xm + ym * ym;
	if (dd > 0) dd = sqrt(dd);
	else dd = 1;
	xm /= dd;
	ym /= dd;

	player->turnBob *= 0.8;
	player->turnBob += player->rota;

	player->bob *= 0.6;
	player->bob += sqrt(xm * xm + ym * ym);
	player->bobPhase += sqrt(xm * xm + ym * ym);

	player->acc.x -= (xm * cos(player->rot) + ym * sin(player->rot)) * movespeed;
	player->acc.y -= (ym * cos(player->rot) - xm * sin(player->rot)) * movespeed;

	double r = 0.1;
	int xs = (player->acc.x > 0) - (player->acc.x < 0);
	int ys = (player->acc.y > 0) - (player->acc.y < 0);
	int newtx = floor(player->pos.x + player->acc.x + r * xs);
	int newty = floor(player->pos.y + player->acc.y + r * ys);
	uint32_t xb = getblock(*level, newtx, ty);
	uint32_t yb = getblock(*level, tx, newty);

	if (xb != 0 && xb != 0xff0000) {
		player->acc.x = 0;
	}
	if (yb != 0 && yb != 0xff0000) {
		player->acc.y = 0;
	}
	// Move the player here.
	player->pos.x += player->acc.x;
	player->pos.y += player->acc.y;

	

	// Apply Friction
	player->acc.x *= 0.6;
	player->acc.y *= 0.6;
	player->rot += player->rota;
	player->rota *= 0.4;

	// int rotsign = SIGN(player->rot);
	if (player->rot > M_PI * 2) player->rot = 0;
	if (player->rot < 0) player->rot += M_PI * 2;
}
