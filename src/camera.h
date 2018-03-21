#ifndef __camera_DEF__
#define __camera_DEF__

#include "point.h"
#include "level.h"

typedef struct {
	double x;
	double y;
	double z;
	double rot;

	// double ax;
	// double vx;

	// double ay;
	// double vy;
	
	// double rot;
	// double rota;
	// double walkSpeed;
	// double speed;
	// double bobPhase;
	// double bob;
	// double turnBob;
} camera_t;


camera_t* camerainit(level_t* level);
// int camera_t_index(lua_State* L);
// int camera_t_newindex(lua_State* L);

#endif