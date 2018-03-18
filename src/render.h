#ifndef __RENDER_DEF__
#define __RENDER_DEF__

#include "context.h"
#include "player.h"
#include "art.h"
#include "level.h"
#include <stdint.h>

void ginit(void);
void setpixel(display_t d, int x, int y, unsigned color);
void renderwall (display_t *d, unsigned* zbuffer, int* zBufferWall, player_t *p, double x0, double y0, double x1, double y1, uint32_t c);
void gprintf(display_t d, int x, int y, const char* format, ...);
void gdrawchar(display_t d, int x, int y, char c);
display_t lock(context_t context);
void unlock(context_t context);
void render(context_t context, player_t* player, level_t level);
void present(context_t context);


unsigned getcolor(unsigned char r, unsigned char g, unsigned char b);
float lerp(int v1, int v2, float alpha);

#endif
