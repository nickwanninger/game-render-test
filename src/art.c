#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "art.h"

extern graphic_t textures;

graphic_t
gopen(char* filename) {
	int i;
	unsigned char tmp;
	FILE* fp = fopen(filename, "r");
	graphic_t g = {NULL, 64, 64};

	// fseek(fp, 0, SEEK_END);
	
	// int filesize = ftell(fp);
	// rewind(fp);
	unsigned char info[54];
	fread(info, sizeof(unsigned char), 54, fp);
	g.width = *(int*)&info[18];
	g.height = *(int*)&info[22];

	int size = 3 * g.width * g.height;
	unsigned char* data = malloc(size);
	// Read size bytes into data
	fread(data, 1, size, fp);
	// Close the file, we are done with it.
	fclose(fp);
	// Convert (G, B, R) to (R, G, B)
	for(i = 0; i < size; i += 3) {
		tmp = data[i];
		data[i] = data[i+2];
		data[i+2] = tmp;
	}
	g.pixels = data;
	return g;
}


unsigned
gpixel(graphic_t* gr, unsigned x, unsigned y) {
	x = gr->width - 1 - x;
	unsigned char r = gr->pixels[3 * (x * gr->width + y) + 0];
	unsigned char g = gr->pixels[3 * (x * gr->width + y) + 1];
	unsigned char b = gr->pixels[3 * (x * gr->width + y) + 2];
	return r << 16 | g << 8 | b;
}

// graphic_t
// gopen(char* filename) {
// 	int i;
// 	// unsigned char tmp;
// 	FILE* fp = fopen(filename, "rb");
// 	graphic_t g = {NULL, 64, 64};
	
// 	unsigned char* info = malloc(54);
// 	fread(info, sizeof(unsigned char), 54, fp);
// 	g.width = *(int*)&info[18];
// 	g.height = *(int*)&info[22];

// 	int size = g.width * g.height;

// 	g.pixels = calloc(4, size);
// 	if (g.pixels == NULL) {
// 		printf("is");
// 	}

// 	char cr = 0;
// 	char cg = 0;
// 	char cb = 0;

// 	for (i = 0; i < size; i++) {
// 		fread(&cb, 1, 1, fp);
// 		fread(&cg, 1, 1, fp);
// 		fread(&cr, 1, 1, fp);
// 		g.pixels[i] = cr << 16 | cg << 8 | cb;
// 	}
// 	free(info);
// 	fclose(fp);
// 	return g;
// }


// uint32_t
// gpixel(graphic_t* gr, unsigned x, unsigned y) {
// 	return gr->pixels[y + x * gr->width];
// }
