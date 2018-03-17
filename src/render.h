#ifndef __RENDER_DEF__
#define __RENDER_DEF__

#include "context.h"
#include "player.h"
#include "art.h"

void ginit(void);
void setpixel(display_t d, int x, int y, unsigned color);
void gprintf(display_t d, int x, int y, const char* format, ...);
void gdrawchar(display_t d, int x, int y, char c);
display_t lock(context_t context);
void unlock(context_t context);
void render(context_t context, player_t* player);
void present(context_t context);

#endif
