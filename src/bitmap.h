#ifndef __ART__
#define __ART__

#include <stdint.h>

typedef struct {
	uint32_t* pixels;
	int width;
	int height;
	double* zBuffer;
	double* zBufferWall;
} display_t;

typedef struct {
	unsigned char* pixels;
	unsigned width;
	unsigned height;
} bitmap_t;


void ginit(void);
bitmap_t* gopen(char* filename);
unsigned gpixel(bitmap_t* g, int x, int y);

uint32_t gloadtexture(char* filename);

#endif
