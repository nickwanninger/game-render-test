#include "level.h"
#include "art.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#define LTILEX(x, width) (x)
#define LTILEY(y, height) (height - x)


int
getblock(level_t l, int y, int x) {
	if (x < 0 || y < 0 || x >= l.width || y >= l.height) {
		return 0x0;
	}
	return l.tiles[x + y * l.width].type;
}




// Open a level file.
level_t*
lopen(char* filename) {
	graphic_t *data = gopen(filename);
	
	int width = data->width;
	int height = data->height;
	// char* tiles = malloc(width * height * 4);

	level_t* level = malloc(sizeof(level_t));
	level->tiles = calloc(width * height, sizeof(level_tile_t));
	level->width = width;
	level->height = height;
	level->pspawn = (point_t) {0.5, 0.5};

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			int color = gpixel(data, y, x);
			int xx = (height - y - 1);
			int yy = x;
			level->tiles[xx + yy * width].type = color;
			if (color == 0x00ff00) {
				printf("set spawn\n");
				level->pspawn.x = yy + 0.5;
				level->pspawn.y = xx + 0.5;
			}
		}
	}

	return level;
}