#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "art.h"
#include "util.h"


graphic_t *textures = NULL;
graphic_t *font_tex = NULL;


void
ginit(void) {
	textures = gopen("assets/textures.bmp");
	font_tex = gopen("assets/font.bmp");
}



graphic_t*
gopen(char* filename) {

	SDL_Surface* surface = SDL_LoadBMP(filename);
	printf("%25s %dx%d\t0x%08x\n", filename, surface->w, surface->h, *(uint32_t*)surface->pixels);
	// int i;
	// FILE* f = fopen(filename, "rb");
	// unsigned char info[54];
	// fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header

	// // extract image height and width from header
	// int width = *(int*)&info[18];
	// int height = *(int*)&info[22];
	// // int bitsperpixel = 3 * 8;
	// int padding=0; while ((width*3+padding) % 4 !=0) padding++;
	// // int rowsize = floor((bitsperpixel * width + 31) / 32) * 4;
	// // printf("%s bpp: %d, padding: %d, rowsize: %d\n",filename, bitsperpixel, padding, rowsize);

	// int size = 3 * width * height;
	// unsigned char* data = xmalloc(size); // allocate 3 bytes per pixel
	// fread(data, sizeof(unsigned char), size, f); // read the rest of the data at once
	// fclose(f);
	int i;
	int size = surface->w * surface->h;

	
	graphic_t* g = xmalloc(sizeof(graphic_t));
	g->width = surface->w;
	g->height = surface->h;
	g->pixels = (unsigned char*) surface->pixels;
	for(i = 0; i < size; i += 3) {
		unsigned char tmp = g->pixels[i];
		g->pixels[i] = g->pixels[i+2];
		g->pixels[i+2] = tmp;
	}
	return g;
}


unsigned
gpixel(graphic_t* gr, int x, int y) {
	// x = gr->height - 1 - x;
	if (x >= 0 && x < gr->width && y >= 0 && y < gr->height) {
		unsigned char r = gr->pixels[3 * (x * gr->width + y) + 0];
		unsigned char g = gr->pixels[3 * (x * gr->width + y) + 1];
		unsigned char b = gr->pixels[3 * (x * gr->width + y) + 2];
		return r << 16 | g << 8 | b;
	}
	return 0xff0000;
}