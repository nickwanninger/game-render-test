#ifndef __RENDER_DEF__
#define __RENDER_DEF__

#include "camera.h"
#include "bitmap.h"
#include "level.h"
#include "game.h"

#include <math.h>
#include <stdint.h>

#include "lua/lua.h"
#include <lualib.h>
#include <lauxlib.h>


#define DRAW_TYPE_WALL     0
#define DRAW_TYPE_SPRITE   1


typedef struct {
	double x0;
	double y0;
	double x1;
	double y1;
	uint32_t c;
} wallinstr_t;

typedef struct {
	double x;
	double y;
	double z;
	int texx;
	int texy;
} spriteinstr_t;

typedef struct {
	int type;
	union {
		wallinstr_t wall;
		spriteinstr_t sprite;
	} instr;
} drawqueueitem_t;

typedef struct {
	int drawalloc;
	int drawc;
	drawqueueitem_t* drawv;
} drawqueue_t;


void setpixel(display_t d, int x, int y, unsigned color);

// static inline void renderwall (display_t *d, unsigned* zbuffer, int* zBufferWall, camera_t *p, double x0, double y0, double x1, double y1, uint32_t c);
// static void renderwall (display_t *d, camera_t *cam, double x0, double y0, double x1, double y1, uint32_t c);
void render(game_t* game);

void gprintf(display_t d, int x, int y, const char* format, ...);
void gdrawchar(display_t d, int x, int y, char c);

void renderconsole(game_t* game, display_t d);

// void renderSprite(display_t d, game_t* game, unsigned* zBuffer, double x, double y, double z, int texx, int texy);
void renderSprite(double x, double y, double z, int texx, int texy);
static inline void renderwall (float x0, float z0, float x1, float z1, int texture);

static inline void renderfloortile(float x, float z, int texture);

unsigned getcolor(unsigned char r, unsigned char g, unsigned char b);
float lerp(int v1, int v2, float alpha);

void drawqueuepush(drawqueueitem_t item);
int renderfromqueue(game_t* game);

// LUA BINDINGS
int l_rendersprite(lua_State* L);

#endif
