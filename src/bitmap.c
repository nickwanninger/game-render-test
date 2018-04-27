#include <SDL2/SDL.h>
#include "gl/gllib.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "bitmap.h"
#include "util.h"


bitmap_t *textures = NULL;
bitmap_t *font_tex = NULL;


void
ginit(void) {
	textures = gopen("assets/textures.bmp");
	font_tex = gopen("assets/font.bmp");
}


// uint32_t
// gbind(bitmap_t* texture) {
// 	uint32_t texture;
// 	return texture;
// }


bitmap_t*
gopen(char* filename) {

	SDL_Surface* surface = SDL_LoadBMP(filename);
	printf("%25s %dx%d\t0x%08x\n", filename, surface->w, surface->h, *(uint32_t*)surface->pixels);

	int i;
	int size = surface->w * surface->h;

	
	bitmap_t* g = xmalloc(sizeof(bitmap_t));
	g->width = surface->w;
	g->height = surface->h;
	g->pixels = (unsigned char*) surface->pixels;



	for(i = 0; i < size * 3; i += 3) {
		unsigned char tmp = g->pixels[i];
		g->pixels[i] = g->pixels[i+2];
		g->pixels[i+2] = tmp;
	}
	return g;
}


unsigned
gpixel(bitmap_t* gr, int x, int y) {
	// x = gr->height - 1 - x;
	if (x >= 0 && x < gr->width && y >= 0 && y < gr->height) {
		unsigned char r = gr->pixels[3 * (x * gr->width + y) + 0];
		unsigned char g = gr->pixels[3 * (x * gr->width + y) + 1];
		unsigned char b = gr->pixels[3 * (x * gr->width + y) + 2];
		return r << 16 | g << 8 | b;
	}
	return 0xff0000;
}


uint32_t
gloadtexture(char* filename) {
	uint32_t tid = 0;
	// You should probably use CSurface::OnLoad ... ;)
	//-- and make sure the Surface pointer is good!
	// SDL_Surface* Surface = SDL_LoadBMP(filename);
	bitmap_t* bitmap = gopen(filename);
	
	glGenTextures(1, &tid);
	glBindTexture(GL_TEXTURE_2D, tid);
	
	int mode = GL_RGB;
	
	// if(bitmap->format->BytesPerPixel == 4) {
	// 		mode = GL_RGBA;
	// }
	
	glTexImage2D(GL_TEXTURE_2D, 0, mode, bitmap->width, bitmap->height, 0, mode, GL_UNSIGNED_BYTE, bitmap->pixels);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	return tid;
}
