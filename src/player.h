#ifndef __player_DEF__
#define __player_DEF__

#include "point.h"

typedef struct {
	point3d_t pos;
	point_t vel;
	point_t acc;
	double rot;
	double rota;
	double walkSpeed;
	double bobPhase;
	double bob;
} player_t;


#endif