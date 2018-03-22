#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <math.h>

#include "input.h"
#include "render.h"
#include "script.h"
#include "profiler.h"
#include "util.h"
#include "lautoc.h"




// #define __SINGLE_THREADED_RENDERING__

int microsecondsinsecond = 1000000;
float microsecondsintick = 1000000 / 60.0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

game_t* globalgame;

game_t*
gameinit(char* levelname) {
	
	float scale = 4;
	int width = 270;
	int height = 190;

	SDL_StartTextInput();
	
	char* scriptname = "script.lua";

	game_t* game = xmalloc(sizeof(game_t));
	

	// Setup the lua env
	game->lua = luaL_newstate();
	
	
	// Load the libraries into lua
	luaL_openlibs(game->lua);

	luaA_open(game->lua);

	luainject(game);
	
	luaL_loadfile(game->lua, "src/bindings.lua");
	int lua_statuscode = luaL_loadfile(game->lua, scriptname);
	
	// Lua didnt like the file or failed to read it.
	if ( lua_statuscode != 0 ) {
		if (lua_statuscode == 7) {
			fprintf(stderr, "[LUA] No Lua File found\n");
		} else {
			fprintf(stderr, "[LUA] Error code: %d\n", lua_statuscode);
			fprintf(stderr, "[LUA] Error loading lua file '%s'\n", scriptname);
			fprintf(stderr, "[LUA] FATAL ERROR:\n");
			fprintf(stderr, "[LUA] %s\n\n", lua_tostring(game->lua, -1));
			exit(lua_statuscode);
		}
	}


	game->drawfloor = true;
	game->drawwalls = true;
	game->drawitem = true;
	game->drawfog = true;
	game->luarunning = false;
	game->drawprofiler = false;

	
	game->renderdist = 15;

	game->frames = 0;
	game->fogdist = 40.0;

	game->console = initconsole();


	game->level = lopen(levelname);
	game->context = contextinit(width, height, scale);
	game->camera = camerainit(game->level);
	globalgame = game;
	return game;
}





int gamedone() {
	SDL_Event event;
	return event.type == SDL_QUIT || event.key.keysym.sym == SDLK_ESCAPE;
}




int
lgetblock(lua_State* L) {
	int x = lua_tointeger(L, -2);
	int y = lua_tointeger(L, -1);
	lua_pushinteger(L, getblock(*globalgame->level, x, y));
	return 1;
}


int game_index(lua_State* L) {
	const char* membername = lua_tostring(L, -1);
	game_t* self = globalgame;
	return luaA_struct_push_member_name(L, game_t, membername, self);
}

int game_newindex(lua_State* L) {
	const char* membername = lua_tostring(L, -2);
	game_t* self = globalgame;
	luaA_struct_to_member_name(L, game_t, membername, self, -1);
	return 0;
}


int camera_index(lua_State* L) {
	const char* membername = lua_tostring(L, -1);
	camera_t* self = globalgame->camera;
	return luaA_struct_push_member_name(L, camera_t, membername, self);
}

int camera_newindex(lua_State* L) {
	const char* membername = lua_tostring(L, -2);
	camera_t* self = globalgame->camera;
	luaA_struct_to_member_name(L, camera_t, membername, self, -1);
	return 0;
}

int game_keystate(lua_State* L) {
	const int scancode = lua_tointeger(L, -1);
	if (globalgame->keystate[scancode]) {
		lua_pushboolean(L, 1);
		return 1;
	} else {
		lua_pushboolean(L, 0);
	}
	return 0;
}


void
luainject(game_t* game) {
	lua_State* L = game->lua;

	luaA_struct(L, camera_t);

	luaA_struct_member(L, camera_t, x, double);
	luaA_struct_member(L, camera_t, y, double);
	luaA_struct_member(L, camera_t, z, double);
	luaA_struct_member(L, camera_t, rot, double);

	lua_register(L, "camera_index", camera_index);
	lua_register(L, "camera_newindex", camera_newindex);


	luaA_struct(L, game_t);
	luaA_struct_member(L, game_t, fps, int);
	luaA_struct_member(L, game_t, frames, int);
	luaA_struct_member(L, game_t, renderdist, int);
	luaA_struct_member(L, game_t, drawfloor, bool);
	luaA_struct_member(L, game_t, drawwalls, bool);
	luaA_struct_member(L, game_t, drawitem, bool);
	luaA_struct_member(L, game_t, drawfog, bool);
	luaA_struct_member(L, game_t, fogdist, double);
	luaA_struct_member(L, game_t, drawprofiler, bool);


	lua_register(L, "key_down", game_keystate);
	lua_register(L, "game_index", game_index);
	lua_register(L, "game_newindex", game_newindex);

	lua_register(L, "getblock", lgetblock);


	lua_register(L, "setpixel", l_setpixel);
	lua_register(L, "getpixel", l_getpixel);
	lua_register(L, "rendersprite", l_rendersprite);
}




int t = 0;
void
gameupdate(game_t* game) {
	t++;

	// if (t % 5 == 0) contextresize(game->context, game->context->width, (int)((sin(t / 100.0) + 1) * 50 + 10), game->context->scale);
	handleinput(game);
}





unsigned long
utime() {
	struct timeval tv;
	// gettimeofday(&tv,NULL);
	return tv.tv_usec;
}





void*
gamerenderthread(void *arg) {
	game_t* game = (game_t*) arg;
	while (1) {


		render(game);
		// usleep(100000);
		game->frames++;
	}
	pthread_exit(0);
	return 0;
}





void*
gamelogicthread(void* arg) {
	game_t* game = (game_t*) arg;
	while(1) {
		profilerstarttick();
		pthread_mutex_lock(&mutex);

		// scriptearlyupdate(game);
		#ifdef __SINGLE_THREADED_RENDERING__
		render(game);
		game->frames++;
		#endif


		scriptpreupdate(game);

		gameupdate(game);

		scriptupdate(game);


		usleep(100);
		profilerendtick();

		// scriptlateupdate(game);
		usleep(microsecondsintick);

		

		pthread_mutex_unlock(&mutex);
		
	}
}

void
gamestart(game_t* game) {

	
	globalgame = game;

	lua_pcall(game->lua, 0, 0, 0);

	scriptinit(game);



	pthread_t logicthread;
	pthread_create(&logicthread, NULL, gamelogicthread, game);
	#ifndef __SINGLE_THREADED_RENDERING__
	// Create the render thread.
	pthread_t renderthread;
	pthread_create(&renderthread, NULL, gamerenderthread, game);
	#endif

	// Loop and handle user input and ticking till the user wants to quit.
	inputEventWatcher(game);
	exit(1);
}

