#include "gl/gllib.h"
#include "gl/window.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include "bitmap.h"
#include "render.h"
#include "level.h"
#include "script.h"
#include "color.h"
#include "util.h"
#include "3d/object.h"

#include "profiler.h"

extern int window_width;
extern int window_height;


#define __RENDER_POSTPROCESS__
#define __RENDER_DEBUG_INFO__
#define __RENDER_CROSSHAIR__

#define RENDER_DISTANCE 15
#define PI 3.141592653589793238462643383279502

#define FONT_CHAR_W 6
#define FONT_CHAR_H 7

#define NORTH 0
#define EAST  1
#define SOUTH 2
#define WEST  3

char* directions[] = {"North", "East", "South", "West"};


unsigned textcolor = 0xffffff;

extern pthread_mutex_t mutex;

extern bitmap_t* textures;
extern bitmap_t* font_tex;

game_t* globalgame;
display_t* globaldisplay;
camera_t camera;

drawqueue_t drawqueue = {
	0,
	0,
	NULL,
};



unsigned
getcolor(unsigned char r, unsigned char g, unsigned char b) {
	return r << 16 | g << 8 | b;
}

float
lerp(int v1, int v2, float alpha) {
	return (int) (v1 + alpha * (v2 - v1));
}

unsigned
lerpcolor(unsigned c1, unsigned c2, double alpha) {
	int r1 = (c1 >> 16) & 0xff;
	int g1 = (c1 >>  8) & 0xff;
	int b1 = (c1 >>  0) & 0xff;
	int r2 = (c2 >> 16) & 0xff;
	int g2 = (c2 >>  8) & 0xff;
	int b2 = (c2 >>  0) & 0xff;
	int r = (int) lerp(r1, r2, alpha);
	int g = (int) lerp(g1, g2, alpha);
	int b = (int) lerp(b1, b2, alpha);
	return r << 16 | g << 8 | b;
}

unsigned
colormul(unsigned c, float m) {
	int r = (c >> 16) & 0xff;
	int g = (c >> 8) & 0xff;
	int b = (c >> 0) & 0xff;
	r = (int) (r * m);
	g = (int) (g * m);
	b = (int) (b * m);
	return getcolor(r, g, b);
}


void
render(game_t* game) {


	drawqueue.drawc = 0;

	obj3d_t monkey = obj3dopen("teapot.obj");

	camera = *game->camera;
	profilerstartframe();

	// context_t context = *(game->context);
	level_t level = *(game->level);
	camera_t* camera = game->camera;
	globalgame = game;

	// double rCos = cos(camera->roty);
	// double rSin = sin(camera->roty);
	double xCam = camera->x;
	double yCam = camera->y;
	double zCam = camera->z;

	// printf("hello\n");


	glMatrixMode(GL_PROJECTION);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glLoadIdentity();

	glEnable(GL_MULTISAMPLE);

	gluPerspective(64.0, window_width / (double) window_height, 0.01, 100.0);
	glViewport(0, 0, window_width, window_height);

	float rx = camera->rotx * (180.0 / PI);
	float ry = camera->roty * (180.0 / PI);

	// printf("%f\n", ry);
	glRotatef(rx, 1.0f, 0.0f, 0.0f);
	glRotatef(ry, 0.0f, 1.0f, 0.0f);
	glTranslatef(-xCam, -yCam, zCam);
	glScalef(1.0f, 1.0f, -1.0f);

	for (int i = 0; i < monkey.facec; i++) {
		face_t face = monkey.facev[i];
		if (face.count == 3) {
			glBegin( GL_TRIANGLES );
			for (int v = 0; v < face.count; v++) {
				float c = 1.0f - v / (float) face.count;
				glColor3f(c, c, c);
				glVertex3f(face.vertices[v].position.x, face.vertices[v].position.y, face.vertices[v].position.z);
			}
			glEnd();
		} else {
			glBegin( GL_QUADS );
			for (int v = 0; v < face.count; v++) {
				float c = 1.0f - v / (float) face.count;
				glColor3f(c, c, c);
				glVertex3f(face.vertices[v].position.x, face.vertices[v].position.y, face.vertices[v].position.z);
			}
			glEnd();
		}
	}
	glColor3f(1.0f, 1.0f, 1.0f);

	// Draw the walls
	if (game->drawwalls) {
		int xb = 0;
		int zb = 0;
		int xCenter = (int)floor(xCam);
		int zCenter = (int)floor(zCam);
		for (xb = xCenter - game->renderdist; xb <= xCenter + game->renderdist; xb++) {
			for (zb = zCenter - game->renderdist; zb <= zCenter + game->renderdist; zb++) {
				if (xb >= 0 && zb >= 0 && xb < game->level->width && zb < game->level->height) {

					uint32_t c = getblock(level, xb, zb);
					int xx = xb;
					int zz = zb;

					if (c == 0x000000) {

					}
					renderfloortile(xx, zz, 1);

					if (c == 0xffffff) {
						// WEST
						renderwall(xx, zz, xx, zz + 1, 2);
						// EAST
						renderwall(xx + 1, zz + 1, xx + 1, zz, 2);
						// NORTH
						renderwall(xx, zz + 1, xx + 1, zz + 1, 2);
						// SOUTH
						renderwall(xx + 1, zz, xx, zz, 2);
					}
				}
			}
		}
	}


	glFinish();
	SDL_GL_SwapWindow( window );
	profilerendframe();
}


static void
renderfloortile(float x, float z, int texture) {
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBegin( GL_QUADS );
		glTexCoord2f(0, 0);
		glVertex3f(x + 0.0f, 0.0f, z + 0.0f);
		glTexCoord2f(0, 1);
		glVertex3f(x + 1.0f, 0.0f, z + 0.0f);
		glTexCoord2f(1, 1);
		glVertex3f(x + 1.0f, 0.0f, z + 1.0f);
		glTexCoord2f(1, 0);
		glVertex3f(x + 0.0f, 0.0f, z + 1.0f);
	glEnd();


	glBegin( GL_QUADS );
		glTexCoord2f(0, 0);
		glVertex3f(x + 0.0f, 1.0f, z + 0.0f);
		glTexCoord2f(0, 1);
		glVertex3f(x + 0.0f, 1.0f, z + 1.0f);
		glTexCoord2f(1, 1);
		glVertex3f(x + 1.0f, 1.0f, z + 1.0f);
		glTexCoord2f(1, 0);
		glVertex3f(x + 1.0f, 1.0f, z + 0.0f);
	glEnd();


}





static inline void
renderwall (float x0, float z0, float x1, float z1, int texture) {
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// glUseProgram(1);
	glBegin(GL_QUADS);

	// Top left
	glTexCoord2f(0, 0);
	glVertex3f(x1, 1.0f, z1);

	// Bottom left
	glTexCoord2f(0, 1);
	glVertex3f(x1, 0.0f, z1);

	// Bottom right
	glTexCoord2f(1, 1);
	glVertex3f(x0, 0.0f, z0);
	// Top right
	glTexCoord2f(1, 0);
	glVertex3f(x0, 1.0f, z0);

	glEnd();
}





void
drawqueuepush(drawqueueitem_t item) {

	if (drawqueue.drawc + 1 > drawqueue.drawalloc) {
		drawqueue.drawalloc += 10;
		drawqueue.drawv = realloc(drawqueue.drawv, drawqueue.drawalloc * sizeof(drawqueueitem_t));
	}
	int i = drawqueue.drawc++;
	drawqueue.drawv[i] = item;
}



int
renderfromqueue(game_t* game) {
	int count = 0;
	// if (drawqueue.drawc > drawqueue.drawalloc)
	// 	drawqueue.drawc = drawqueue.drawalloc;
	// for (int i = 0; i < drawqueue.drawc; i++) {
	// 	drawqueueitem_t call = drawqueue.drawv[i];
	// 	switch (call.type) {
	// 		case DRAW_TYPE_WALL:
	// 			break;
	// 		case DRAW_TYPE_SPRITE:
	// 			renderSprite(game,
	// 						call.instr.sprite.x,
	// 						call.instr.sprite.y,
	// 						call.instr.sprite.z,
	// 						call.instr.sprite.texx,
	// 						call.instr.sprite.texy);
	// 			break;
	// 	}
	// }
	// drawqueue.drawc = 0;
	return count;
}








// void
// renderconsole(game_t* game, display_t d) {
// 	int x = 0;
// 	int y = 0;
// 	int i = 0;
// 	int linec = game->console->linec;
// 	char** linev = game->console->linev;
// 	int lineheight = FONT_CHAR_H + 5;
// 	int linestorender = d.height / lineheight - 1;
// 	for (x = 0; x < d.width; x++) {
// 		for (y = 0; y < d.height; y++) {
// 			unsigned color = d.pixels[x + y * d.width];
// 			setpixel(d, x, y, colormul(color, 0.1));
// 		}
// 	}
// 	int currentline = 0;
// 	for (i = linec - linestorender; i < linec; i++) {
// 		if (i >= 0) {
// 			gprintf(d, 2, lineheight * currentline, "%s", linev[i]);
// 			currentline++;
// 		}

// 	}


// 	gprintf(d, 2, lineheight * currentline, ">> %s", game->console->input);
// 	int cursoroffset = strlen(game->console->input) + 3;

// 	if (sin(game->frames / 10.0) < 0) {
// 		gprintf(d, FONT_CHAR_W * cursoroffset, lineheight * currentline, "|");
// 	}
// 	// textcolor = 0xffffff;


// }



/** gprintf
 * takes a <d>isplay, and <x> and a <y> and prints
 * a formatted string to the screen starting at that
 * location.
 *
 * The string starts where the top left of the string
 * is at (<x>, <y>)
 */

void
gprintf(display_t d, int x, int y, const char* format, ...) {
	unsigned long i = 0;
	char buffer[255];
	va_list args;
	va_start(args, format);
	int r = vsnprintf(buffer, 255, format, args);
	buffer[r] = 0;
	va_end(args);
	for (i = 0; i < strlen(buffer); i++)
		gdrawchar(d, x + (i * FONT_CHAR_W), y, buffer[i]);
}

/** gdrawchar
 * Takes a <d>isplay, an <x>, a <y> and draws the char <c>
 * to that display at that x, y location
 */
void
gdrawchar(display_t d, int x, int y, char c) {
	// int w = 6;
	// int h = 8;

	// if (c >= 32 && c < 127) {
	// 	char co = c - 32;
	// 	// int offset = co * w;
	// 	for (int xo = 0; xo < w; xo++) {
	// 		for (int yo = 0; yo < h; yo++) {
	// 			int xx = xo + (co % 16) * w;
	// 			int yy = yo + co / 16 * h;
	// 			int pixel = gpixel(font_tex, yy, xx);
	// 			if (pixel == 0xffffff) {
	// 				setpixel(d, x + xo, y + yo, textcolor);
	// 				setpixel(d, x + xo, y + yo + 1, colormul(textcolor, 0.4));
	// 				// setpixel(d, x + xo + 2, y + yo + 2, colormul(textcolor, 0.4));
	// 			}
	// 		}
	// 	}

	// }
}

int l_rendersprite(lua_State* L) {
	int texy = (int)lua_tointeger(L, -1);
	int texx = (int)lua_tointeger(L, -2);
	double z = (double)lua_tonumber(L, -3);
	// printf("%f\t", z);
	double y = (double)lua_tonumber(L, -4);
	// printf("%f\t", y);
	double x = (double)lua_tonumber(L, -5);

	drawqueueitem_t item = {};
	item.type = DRAW_TYPE_SPRITE;
	item.instr.sprite.x = x;
	item.instr.sprite.y = y;
	item.instr.sprite.z = z;
	item.instr.sprite.texx = texx;
	item.instr.sprite.texy = texy;

	drawqueuepush(item);

	return 1;
}





void
renderSprite(double x, double y, double z, int texx, int texy) {

}
