#ifndef __LEVEL__
#define __LEVEL__

#include "point.h"
#include <stdint.h>

typedef struct {
	uint32_t* tiles;
	int width;
	int height;
	point_t pspawn;
} level_t;

int getblock(level_t l, int x, int y);
level_t lopen(char* filename);

#endif