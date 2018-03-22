#ifndef __GAME__
#define __GAME__

#include "context.h"
#include "camera.h"
#include "level.h"
#include "console.h"



#include <stdint.h>
#include <stdbool.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>


typedef struct {
	context_t* context;
	camera_t* camera;
	level_t* level;
	console_t* console;
	lua_State* lua;
	int width;
	int height;
	uint64_t frames;
	uint64_t ticks;
	int fps;
	int tps;
	uint8_t* keystate;
	bool drawwalls;
	bool drawfog;
	bool drawfloor;
	bool drawitem;
	bool luarunning;
	bool drawprofiler;
	int renderdist;

	double fogdist;
} game_t;

// #include "luabinding.h"


// Function to initialize a game and return a pointer to that game
game_t* gameinit(char* levelname);
void gameupdate(game_t* game);
void gamestart(game_t* game);
int gamedone();

void luainject(game_t* game);
#endif