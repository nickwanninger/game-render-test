#include "gl/gllib.h"
#include "gl/window.h"
#include "gl/context.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"
#include <math.h>
#include <SDL2/SDL.h>
#include "bitmap.h"
#include "input.h"
#include "render.h"
#include "script.h"
#include "profiler.h"
#include "util.h"
#include "lautoc.h"
#include "shader.h"
#include "3d/object.h"



int microsecondsinsecond = 1000000;
float microsecondsintick = 1000000 / 60.0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

game_t* globalgame;

game_t*
gameinit(char* levelname) {

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
	game->ticks = 0;
	game->fogdist = 40.0;
	game->console = initconsole();
	game->level = lopen(levelname);
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
	luaA_struct_member(L, camera_t, rotx, double);
	luaA_struct_member(L, camera_t, roty, double);

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


	lua_register(L, "rendersprite", l_rendersprite);
}




int t = 0;
void
gameupdate(game_t* game) {
	handleinput(game);
}





void*
gamerenderthread(void *arg) {

	game_t* game = (game_t*) arg;
	windowinit();
	contextinit();
	
	if(!initGL()) {
		printf( "Unable to initialize OpenGL!\n" );
		return 0;
	}
	printf("Initialized!\n");
	printf("Texture: %u\n", gloadtexture("assets/floor.bmp"));
	printf("Texture: %u\n", gloadtexture("assets/wall.bmp"));


	unsigned int shader = openshader("shader.vert", "shader.frag");
	printf("%d\n", shader);
	glUseProgram(shader);

	uint64_t dt = SDL_GetPerformanceFrequency();

	uint64_t t0, t1;
	int fts = 0; // Frames this second
	while (1) {
		game->frames++;
		fts++;
		t0 = SDL_GetPerformanceCounter();
		render(game);
		t1 = SDL_GetPerformanceCounter();
		if (game->frames % 20 == 0) game->fps = dt / (t1 - t0);
	}
	pthread_exit(0);
	return 0;
}







void*
gamelogicthread(void* arg) {
	game_t* game = (game_t*) arg;

	uint64_t dt = SDL_GetPerformanceFrequency();
	uint64_t t0, t1;

	while(1) {
		profilerstarttick();
		pthread_mutex_lock(&mutex);



		game->ticks++;

		t0 = SDL_GetPerformanceCounter();
		scriptpreupdate(game);

		gameupdate(game);

		scriptupdate(game);

		usleep(microsecondsintick);

		t1 = SDL_GetPerformanceCounter();
		if (game->ticks % 20 == 0) game->tps = dt / (t1 - t0);


		pthread_mutex_unlock(&mutex);
		profilerendtick();
		
	}
}

void
gamestart(game_t* game) {

	
	globalgame = game;

	lua_pcall(game->lua, 0, 0, 0);

	scriptinit(game);



	pthread_t logicthread;
	pthread_create(&logicthread, NULL, gamelogicthread, game);
	
	// Create the render thread.
	pthread_t renderthread;
	pthread_create(&renderthread, NULL, gamerenderthread, game);
	
	// Loop and handle user input and ticking till the user wants to quit.
	inputEventWatcher(game);

	
	exit(1);
}

