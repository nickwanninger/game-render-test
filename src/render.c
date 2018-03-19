#include "render.h"
#include "level.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <pthread.h>

#define __RENDER_POSTPROCESS__

#define PI 3.141592653589793238462643383279502

#define NORTH 0
#define EAST  1
#define SOUTH 2
#define WEST  3

char* directions[] = {"North", "East", "South", "West"};



extern long frames;
extern long ticks;
extern int width;
extern int height;
extern float scale;
extern float pixelscale;

extern int fps;


graphic_t *textures = NULL;
graphic_t *font_tex = NULL;


void
ginit(void) {
	font_tex = gopen("assets/font.bmp");
	textures = gopen("assets/textures.bmp");
}

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
render(context_t context, player_t* player, level_t level) {
	display_t display = lock(context);

	int height = context.h;
	int width = context.w;
	

	unsigned zBuffer[width * height];
	int zBufferWall[width];
	for (int i = 0; i < width; i++) zBufferWall[i] = 0;
	

	double rCos = cos(player->rot);
	double rSin = sin(player->rot);
	double xCam = player->pos.x - 0.5;
	double yCam = player->pos.y - 0.5;

	player->pos.z = 0.1 + sin(player->bobPhase * 0.4) * 0.01 * player->bob;
	double zCam = player->pos.z;

	int dir = (int) ((2 * player->rot) / PI + 1.5) % 4;

	int fov = height;

	int y = 0;
	int x = 0;

	double yCenter = height / 2;

	for (y = 0; y < height; y++) {
		double yd = ((y + 0.5) - yCenter) / fov;
		double zd = (4 + zCam * 8) / yd;
		if (yd < 0) {
			zd = (4 - zCam * 8) / -yd;
		}

		for (x = 0; x < width; x++) {
			double xd = (x - width / 2.0) / height;
			xd *= zd;

			double xx = xd * rCos + zd * rSin + (xCam + 0.5) * 8;
			double yy = zd * rCos - xd * rSin + (yCam + 0.5) * 8;
			// printf("1\n");
			int xPix = (int) (xx * 2);
			int yPix = (int) (yy * 2);
			if (xx < 0) xPix--;
			if (yy < 0) yPix--;

			int texRow = 16;
			int texCol = 0;

			zBuffer[x + y * width] = zd;

			unsigned color = color = gpixel(textures, texRow + (xPix & 15), texCol + (yPix & 15));
			display.pixels[x + y * width] = color;
		}
	}
	
	

	int xb = 0;
	int yb = 0;
	int r = 16;
	int wallcount = 0;

	int xCenter = (int)floor(xCam);
	int zCenter = -(int)floor(yCam);
	
	for (xb = xCenter - r; xb <= xCenter + r; xb++) {
		for (yb = zCenter - r; yb <= zCenter + r; yb++) {
			uint32_t c = getblock(level, xb, -yb);
			
			if (c != 0x000000 && c != 0xff0000) {
				
				// North
				if (dir != NORTH && getblock(level, xb - 1, -yb) == 0) {
					wallcount++;
					renderwall(&display, zBuffer, zBufferWall, player, xb, yb, xb, yb + 1, c);
				}
				// South
				if (dir != SOUTH && getblock(level, xb + 1, -yb) == 0) {
					wallcount++;
					renderwall(&display, zBuffer, zBufferWall, player, xb + 1, yb + 1, xb + 1, yb, c);
				}
				// East
				if (dir != EAST && getblock(level, xb, -yb + 1) == 0) {
					wallcount++;
					renderwall(&display, zBuffer, zBufferWall, player, xb + 1, yb, xb, yb, c);
				}
				// West
				if (dir != WEST && getblock(level, xb, -yb - 1) == 0) {
					wallcount++;
					renderwall(&display, zBuffer, zBufferWall, player, xb, yb + 1, xb + 1, yb + 1, c);
				}
			}
		}
	}

	

	#ifdef __RENDER_POSTPROCESS__
	int fogdist = 30 * 7;
	int ditherfactor = 9;
	for (int i = 0; i < width * height; i++) {
		int xp = (i % width);
		int yp = (i / width) * 14;
		double zl = zBuffer[i];
		double xx = ((i % width - width / 2.0) / width);
		int col = display.pixels[i];
		int brightness = (int) (fogdist - zl * 6 * (xx * xx * 2 + 1));
		brightness = (brightness + ((xp + yp) & 2) * ditherfactor) >> 4 << 4;
		if (brightness < 0) brightness = 0;
		if (brightness > fogdist) brightness = fogdist;
		int r = (col >> 16) & 0xff;
		int g = (col >>  8) & 0xff;
		int b = (col >>  0) & 0xff;
		r = r * brightness / fogdist;
		g = g * brightness / fogdist;
		b = b * brightness / fogdist;
		display.pixels[i] = r << 16 | g << 8 | b;
	}
	#endif


	// for (int i = 0; i < width * height; i++) display.pixels[i] = getcolor(100 - zBuffer[i], 100 - zBuffer[i], 100 - zBuffer[i]);

	int tcol = 32;
	int trow = 16;
	int texsize = 16;
	int s = 4;
	double scalesqrt = sqrt(s);
	int xx = (int) (-player->turnBob * 32) * scalesqrt;
	int yy = (int) (sin(player->bobPhase * 0.4) * 1 * player->bob + player->bob * 2) * scalesqrt;
	xx += width / 1.8;
	yy += height - texsize * s;
	yy += abs((int) ((-player->turnBob * 12) * scalesqrt));
	xx += (int) (sin(player->bobPhase * 0.2) * 1 * player->bob + player->bob * 2) * scalesqrt;
	for (x = 0; x < texsize * s; x++) {
		int xPix = x + xx;
		for (y = 0; y < texsize * s; y++) {
			int yPix = y + yy;
			unsigned color = gpixel(textures, y/s + tcol, x/s + trow);
			if (color == 0xff00ff) continue;
			setpixel(display, xPix, yPix, color);
		}
	}

	// Draw crosshair
	for (int i = 0; i < 5; i++) setpixel(display, (width / 2), height / 2 - 2 + i, 0xffffff);
	for (int i = 0; i < 5; i++) setpixel(display, width / 2 - 2 + i, (height / 2), 0xffffff);

	gprintf(display, 2, 2,  "%f", player->rot);
	gprintf(display, 2, 11, "Dir: %s", directions[dir]);
	gprintf(display, 2, 19, "Walls: %d", wallcount);


	unlock(context);
	present(context);
}




static inline void
renderwall (display_t *d, unsigned* zbuffer, int* zBufferWall, player_t *p, double x0, double y0, double x1, double y1, uint32_t c) {
	
	double xCam = p->pos.x - 0.5;
	double yCam = -p->pos.y + 0.5;
	double zCam = -p->pos.z;
	double rCos = -cos(p->rot);
	double rSin = sin(p->rot);
	double xCenter = width / 2;
	double yCenter = height / 2;
	int fov = height;
	double xc0 = ((x0 - 0.5) - xCam) * 2;
	double yc0 = ((y0 - 0.5) - yCam) * 2;

	double xx0 = xc0 * rCos - yc0 * rSin;
	double u0 = ((-0.5) - zCam) * 2;
	double l0 = ((+0.5) - zCam) * 2;
	double zz0 = yc0 * rCos + xc0 * rSin;

	double xc1 = ((x1 - 0.5) - xCam) * 2;
	double yc1 = ((y1 - 0.5) - yCam) * 2;

	double xx1 = xc1 * rCos - yc1 * rSin;
	double u1 = ((-0.5) - zCam) * 2;
	double l1 = ((+0.5) - zCam) * 2;
	double zz1 = yc1 * rCos + xc1 * rSin;

	double xt0 = 0;
	double xt1 = 1;

	xt0 *= 16;
	xt1 *= 16;

	int tex = 0;
	int col = 0;

	double zClip = 0.1;

	if (zz0 < zClip && zz1 < zClip)
		return;

	if (zz0 < zClip) {
		double p = (zClip - zz0) / (zz1 - zz0);
		zz0 = zz0 + (zz1 - zz0) * p;
		xx0 = xx0 + (xx1 - xx0) * p;
		xt0 = xt0 + (xt1 - xt0) * p;
	}

	if (zz1 < zClip) {
		double p = (zClip - zz0) / (zz1 - zz0);
		zz1 = zz0 + (zz1 - zz0) * p;
		xx1 = xx0 + (xx1 - xx0) * p;
		xt1 = xt0 + (xt1 - xt0) * p;
	}

	double xPixel0 = xCenter - (xx0 / zz0 * fov);
	double xPixel1 = xCenter - (xx1 / zz1 * fov);

	if (xPixel0 >= xPixel1)
		return;
	int xp0 = (int) ceil(xPixel0);
	int xp1 = (int) ceil(xPixel1);
	if (xp0 < 0)
		xp0 = 0;
	if (xp1 > width)
		xp1 = width;

	double yPixel00 = (u0 / zz0 * fov + yCenter);
	double yPixel01 = (l0 / zz0 * fov + yCenter);
	double yPixel10 = (u1 / zz1 * fov + yCenter);
	double yPixel11 = (l1 / zz1 * fov + yCenter);

	double iz0 = 1 / zz0;
	double iz1 = 1 / zz1;

	double iza = iz1 - iz0;

	double ixt0 = xt0 * iz0;
	double ixta = xt1 * iz1 - ixt0;
	double iw = 1 / (xPixel1 - xPixel0);
	

	for (int x = xp0; x < xp1; x++) {
		double pr = (x - xPixel0) * iw;
		double iz = iz0 + iza * pr;
		double zd = iz * 255;
		if (zBufferWall[x] > zd) {
			continue;
		}
		zBufferWall[x] = zd;
		int xTex = (int) ((ixt0 + ixta * pr) / iz);

		double yPixel0 = yPixel00 + (yPixel10 - yPixel00) * pr - 0.5;
		double yPixel1 = yPixel01 + (yPixel11 - yPixel01) * pr;

		int yp0 = (int) ceil(yPixel0);
		int yp1 = (int) ceil(yPixel1);
		if (yp0 < 0)
			yp0 = 0;
		if (yp1 > height)
			yp1 = height;

		double ih = 1 / (yPixel1 - yPixel0);
		for (int y = yp0; y < yp1; y++) {
			double pry = (y - yPixel0) * ih;
			int yTex = (int) (16 * pry);
			int xt = yTex + (tex % 8) * 16;
			int yt = xTex + (col / 8) * 16;
			int color = gpixel(textures, xt, yt);
			d->pixels[x + y * width] = color;
			zbuffer[x + y * width] = 1 / iz * 4;
		}
	}
}






/** gprintf
 * takes a <d>isplay, and <x> and a <y> and prints
 * a formatted string to the screen starting at that
 * location.
 *
 * The string starts where the top left of the string
 * is at (<x>, <y>)
 */
#define FONT_CHAR_W 6
#define FONT_CHAR_H 7
void
gprintf(display_t d, int x, int y, const char* format, ...) {
	int i = 0;
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
	int w = 6;
	int h = 8;

	if (c >= 32 && c < 127) {
		char co = c - 32;
		// int offset = co * w;
		for (int xo = 0; xo < w; xo++) {
			for (int yo = 0; yo < h; yo++) {
				int xx = xo + (co % 16) * w;
				int yy = yo + co / 16 * h;
				int pixel = gpixel(font_tex, yy, xx);
				if (pixel == 0xffffff) {
					setpixel(d, x + xo, y + yo, 0xffffff);
					setpixel(d, x + xo, y + yo + 1, 0x000000);
				}
			}
		}
		
	}
}


void
setpixel(display_t d, int x, int y, unsigned color) {
	if (x < d.width && y < height)
		d.pixels[x + y * d.width] = color;
}


display_t
lock(context_t context) {
	void* screen;
	int pitch;
	SDL_LockTexture(context.texture, NULL, &screen, &pitch);
	display_t display = { (uint32_t*) screen, pitch / (int) sizeof(uint32_t) };
	return display;
}


void unlock(context_t context) {
	SDL_UnlockTexture(context.texture);
}


void present(context_t context) {
	int w = (int) (context.w * context.scale);
	int h = (int) (context.h * context.scale);
	SDL_Rect dst = { 0, 0, w, h, };
	SDL_RenderCopyEx(context.renderer, context.texture, NULL, &dst, 0, NULL, SDL_FLIP_NONE);
	SDL_RenderPresent(context.renderer);
}
