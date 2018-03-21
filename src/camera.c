#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <stdlib.h>
#include "camera.h"
#include "level.h"
#include "point.h"
#include "lautoc.h"

camera_t*
camerainit(level_t* level) {
	camera_t* p = calloc(sizeof(camera_t), 1);
	p->x = level->pspawn.x;
	p->y = level->pspawn.y;
	p->z = 0;
	return p;
}


