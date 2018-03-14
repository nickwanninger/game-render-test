#ifndef __RENDER_DEF__
#define __RENDER_DEF__

#include "context.h"
#include "player.h"
#include "art.h"


void setpixel(display_t d, int x, int y, unsigned color);

display_t lock(context_t context);
void unlock(context_t context);
void render(context_t context, player_t* player, graphic_t texture);
void present(context_t context);

#endif