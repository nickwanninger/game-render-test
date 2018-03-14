#include "input.h"
#include "player.h"
#include <SDL2/SDL.h>

void
handleinput(player_t* player) {
	const uint8_t* key = SDL_GetKeyboardState(NULL);

	double xm = player->vel.x;
	double ym = player->vel.y;
	// Rotation
	if(key[SDL_SCANCODE_LEFT]) player->rota -= 0.025f;
	if(key[SDL_SCANCODE_RIGHT]) player->rota += 0.025f;

	if(key[SDL_SCANCODE_W]) ym--;
	if(key[SDL_SCANCODE_S]) ym++;
	if(key[SDL_SCANCODE_D]) xm--;
	if(key[SDL_SCANCODE_A]) xm++;

	player->bob *= 0.6;
	player->bob += sqrt(xm * xm + ym * ym);
	player->bobPhase += sqrt(xm * xm + ym * ym);

	double dd = xm
	 * xm + ym * ym;
	if (dd > 0) dd = sqrt(dd);
	else dd = 1;
	xm /= dd;
	ym /= dd;

	player->acc.x -= (xm * cos(player->rot) + ym * sin(player->rot)) * player->walkSpeed;
	player->acc.y -= (ym * cos(player->rot) - xm * sin(player->rot)) * player->walkSpeed;

	// Move the player here.
	player->pos.x += player->acc.x;
	player->pos.y += player->acc.y;

	// Apply Friction
	player->acc.x *= 0.8;
	player->acc.y *= 0.8;
	player->rot += player->rota;
	player->rota *= 0.7;
}