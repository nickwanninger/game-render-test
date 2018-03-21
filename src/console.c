#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "util.h"

#include "console.h"


#define lua_saveline(L,line)	{ (void)L; (void)line; }

lua_State* globalL;


console_t* initconsole() {
	console_t* c = xmalloc(sizeof(console_t));
	c->active = false;
	// Allocate 255 bytes to the input.
	c->input = calloc(CONSOLE_INPUT_LENGTH, 1);
	// initialize the input with a null terminator
	c->input[0] = 0;

	c->linespace = 10;
	c->linev = calloc(sizeof(char*), 10);
	c->linec = 0;

	// for (int i = 0; i < CONSOLE_LINE_COUNT; i++) {
	// 	c->lines[i] = calloc(CONSOLE_INPUT_LENGTH, 1);
	// }
	return c;
}

int consoletoggle(console_t* console) {
	console->active = console->active ? false : true;
	return console->active;
}

int consoleinsertline(console_t* console, char* line) {
	return 0;
}

int consoleinputaddchar(console_t* console, char c) {
	int len = strnlen(console->input, CONSOLE_INPUT_LENGTH);
	console->input[len] = c;
	return 0;
}

int consoleinputdelchar(console_t* console) {
	int len = strlen(console->input);
	console->input[len - 1] = 0;
	return 0;
}

int consoleinputclear(console_t* console) {
	for (int i = 0; i < CONSOLE_INPUT_LENGTH; i++) {
		console->input[i] = 0;
	}
	return 0;
}



int
consolepushline(console_t* console, char* line) {
	if (console->linec + 1 > console->linespace) {
		console->linespace *= 2;
		console->linev = realloc(console->linev, console->linespace * sizeof(char*));
	}

	int i = console->linec++;
	console->linev[i] = xmalloc(CONSOLE_INPUT_LENGTH);

	memcpy(console->linev[i], line, CONSOLE_INPUT_LENGTH - 1);
	return 1;
}


// static void l_print (lua_State *L) {
//   int n = lua_gettop(L);
//   if (n > 0) {  /* any result to be printed? */
//     luaL_checkstack(L, LUA_MINSTACK, "too many results to print");
//     lua_getglobal(L, "print");
//     lua_insert(L, 1);
//     if (lua_pcall(L, n, 0, 0) != LUA_OK)
//       printf("error calling 'print' (%s)", lua_tostring(L, -1));
//   }
// }



// static void lstop (lua_State *L, lua_Debug *ar) {
//   (void)ar;  /* unused arg. */
//   lua_sethook(L, NULL, 0, 0);  /* reset hook */
//   luaL_error(L, "interrupted!");
// }

// static void laction (int i) {
//   signal(i, SIG_DFL); /* if another SIGINT happens, terminate process */
//   lua_sethook(globalL, lstop, LUA_MASKCALL | LUA_MASKRET | LUA_MASKCOUNT, 1);
// }


// static int msghandler (lua_State *L) {
//   const char *msg = lua_tostring(L, 1);
//   if (msg == NULL) {  /* is error object not a string? */
//     if (luaL_callmeta(L, 1, "__tostring") &&  /* does it have a metamethod */
//         lua_type(L, -1) == LUA_TSTRING)  /* that produces a string? */
//       return 1;  /* that is the message */
//     else
//       msg = lua_pushfstring(L, "(error object is a %s value)",
//                                luaL_typename(L, 1));
//   }
//   luaL_traceback(L, L, msg, 1);  /* append a standard traceback */
//   return 1;  /* return the traceback */
// }


int
consoleinputeval(console_t* console, lua_State* L) {

	lua_settop(L, 0);

	int status;
	char* command = xmalloc(CONSOLE_INPUT_LENGTH + 1);
	memcpy(command, console->input, CONSOLE_INPUT_LENGTH);
	status = luaL_dostring(L, command);

	printf("error calling 'print' (%s)\n", lua_tostring(L, 0));

	consoleinputclear(console);
	consolepushline(console, command);

	// size_t l = strlen(command);
	// if (l > 0 && command[l-1] == '\n')  /* line ends with newline? */
  //   command[--l] = '\0'; // Remove it.

	// if (command[0] == '=') {
	// 	lua_pushfstring(L, "return %s", command + 1);  /* change '=' to 'return' */
	// } else {
	// 	lua_pushlstring(L, command, l);
	// }


	// const char *line = lua_tostring(L, -1);
	// const char *retline = lua_pushfstring(L, "return %s;", line);
	// status = luaL_loadbuffer(L, retline, strlen(retline), "=stdin");
	// if (status == LUA_OK) {
	// 	lua_remove(L, -2);  /* remove modified line */
	// 	if (line[0] != '\0')  /* non empty? */
	// 		lua_saveline(L, line);  /* keep history */
	// }
	// else
	// 	lua_pop(L, 2);

	// lua_remove(L, 1);  /* remove line from the stack */
	// lua_assert(lua_gettop(L) == 1);

	// int narg = 0;
	// int nres = LUA_MULTRET;
	// int base = lua_gettop(L) - narg;  /* function index */
  // lua_pushcfunction(L, msghandler);  /* push message handler */
  // lua_insert(L, base);  /* put it under function and args */
  // globalL = L;  /* to be available to 'laction' */
  // signal(SIGINT, laction);  /* set C-signal handler */
  // status = lua_pcall(L, narg, nres, base);
  // signal(SIGINT, SIG_DFL); /* reset C-signal handler */
  // lua_remove(L, base);  /* remove message handler from the stack */

	
	// if (status == LUA_OK) {
	// 	l_print(L);
	// }
	// else {
	// 	printf("error: (%s)\n", lua_tostring(L, -1));
	// }



	
	
	return 0;
}