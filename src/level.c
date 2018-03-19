#include "level.h"
#include "art.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


int getblock(level_t l, int y, int x) {
	if (x < 0 || y < 0 || x >= l.width || y >= l.height) {
		return 0x0;
	}
	return l.tiles[x + y * l.width];
}


level_t
lopen(char* filename) {
	graphic_t *data = gopen(filename);
	
	int width = data->width;
	int height = data->height;
	char* tiles = malloc(width * height * 4);

	level_t level = {tiles, width, height, {0.0, 0.0}};
	

	for (int x = 0; x < data->height; x++) {
		for (int y = 0; y < data->width; y++) {
			level.tiles[x + y * width] = gpixel(data, width - x, y);
			if (gpixel(data, x, y) == 0x00ff00) {
				level.pspawn.x = (float) y + 0.5;
				level.pspawn.y = (float) (width - x) + 0.5;
			}
		}
	}
	// printf("%f %f\n", level.pspawn.x, level.pspawn.y);
	return level;
}