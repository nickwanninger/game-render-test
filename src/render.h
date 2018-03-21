#ifndef __RENDER_DEF__
#define __RENDER_DEF__

#include "context.h"
#include "camera.h"
#include "art.h"
#include "level.h"
#include "game.h"
#include <stdint.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

void ginit(void);
void setpixel(display_t d, int x, int y, unsigned color);

static inline void renderwall (display_t *d, unsigned* zbuffer, int* zBufferWall, camera_t *p, double x0, double y0, double x1, double y1, uint32_t c);

void gprintf(display_t d, int x, int y, const char* format, ...);
void gdrawchar(display_t d, int x, int y, char c);
display_t lock(context_t context);
void unlock(context_t context);
void render(game_t* game);
void present(context_t context);
void renderconsole(game_t* game, display_t d);

void renderSprite(display_t d, game_t* game, unsigned* zBuffer, double x, double y, double z, int texx, int texy);

unsigned getcolor(unsigned char r, unsigned char g, unsigned char b);
unsigned getpixel(display_t d, int x, int y);
float lerp(int v1, int v2, float alpha);

// LUA BINDINGS
int l_setpixel(lua_State* L);
int l_getpixel(lua_State* L);

#endif
