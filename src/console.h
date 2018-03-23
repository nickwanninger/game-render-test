#ifndef __CONSOLE__
#define __CONSOLE__

#define CONSOLE_LINE_COUNT 30
#define CONSOLE_INPUT_LENGTH 255



typedef struct {
	// char* lines[CONSOLE_LINE_COUNT];
	char** linev;
	int linec;
	int linespace;
	char* input;
	int active;
} console_t;


#include "lua/lua.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"

console_t* initconsole();
int consoletoggle(console_t* console);
int consoleinsertline(console_t* console, char* line);
int consoleinputaddchar(console_t* console, char c);
int consoleinputdelchar(console_t* console);
int consoleinputclear(console_t* console);
int consoleinputeval(console_t* console, lua_State* lua);

#endif