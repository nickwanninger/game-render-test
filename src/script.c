#include <stdbool.h>
#include <unistd.h>
#include <stdarg.h>
#include "script.h"
#include "game.h"
#include "profiler.h"


int c = 0;

void
scriptcall(game_t* game, char* func) {
	// This luarunning stuff is because of the
	// chance that c will call this in two different
	// Threads
	while (game->luarunning == true) usleep(10); // Sleep and check again.
	game->luarunning = true;
	profilerstartlua();
	lua_getglobal(game->lua, func);
	int statuscode = lua_pcall(game->lua, 0, 0, 0);
	profilerendlua();
	if (statuscode != LUA_OK) {
		fprintf(stderr, "[LUA FUNCTION: %s] %s\n", func, lua_tostring(game->lua, -1));
	}
	game->luarunning = false;
}



void
scriptonkeydown(game_t* game, int code) {
	// This luarunning stuff is because of the
	// chance that c will call this in two different
	// Threads
	while (game->luarunning == true) usleep(10); // Sleep and check again.
	game->luarunning = true;
	profilerstartlua();
	lua_getglobal(game->lua, "onKeyDown");
	lua_pushinteger(game->lua, code);   /* push 1st argument */
	int statuscode = lua_pcall(game->lua, 1, 0, 0);
	profilerendlua();
	if (statuscode != LUA_OK) {
		fprintf(stderr, "[LUA FUNCTION: %s] %s\n", "onKeyDown", lua_tostring(game->lua, -1));
	}
	game->luarunning = false;
}



void
scriptinit(game_t* game) {
	scriptcall(game, "init");
}

void
scriptdraw(game_t* game) {
	scriptcall(game, "draw");
}

void
scriptupdate(game_t* game) {
	scriptcall(game, "update");
}

void
scriptpreupdate(game_t* game) {
	scriptcall(game, "preupdate");
}