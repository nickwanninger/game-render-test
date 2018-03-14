#ifndef __ART__
#define __ART__


typedef struct {
	uint32_t* pixels;
	int width;
} display_t;

typedef struct {
	unsigned char* pixels;
	unsigned width;
	unsigned height;
} graphic_t;


graphic_t gopen(char* filename);
unsigned gpixel(graphic_t* g, unsigned x, unsigned y);

#endif