#ifndef __SCRIPT__
#define __SCRIPT__

// #include "lua/lua.h"
// #include "lua/lualib.h"
// #include "lua/lauxlib.h"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "game.h"

void scriptinit(game_t* game);
void scriptcall(game_t* game, char* func);
void scriptdraw(game_t* game);
void scriptupdate(game_t* game);
void scriptpreupdate(game_t* game);
void scriptonkeydown(game_t* game, int code);


#endif