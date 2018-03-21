#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include "context.h"
#include "render.h"
#include "level.h"
#include "script.h"


#include "profiler.h"


#define __RENDER_WALLS__
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

graphic_t *textures = NULL;
graphic_t *font_tex = NULL;

display_t* globaldisplay;


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
render(game_t* game) {
	
	profilerstartframe();

	context_t context = *(game->context);
	level_t level = *(game->level);
	camera_t* camera = game->camera;
	display_t display = lock(context);
	globaldisplay = &display;
	

	unsigned zBuffer[WIDTH * HEIGHT];
	int zBufferWall[WIDTH];
	for (int i = 0; i < WIDTH; i++) zBufferWall[i] = 0;

	// for (int i = 0; i < WIDTH * HEIGHT; i++) display.pixels[i] = 0;
	
	

	double rCos = cos(camera->rot);
	double rSin = sin(camera->rot);
	double xCam = camera->x - 0.5;
	double yCam = camera->y - 0.5;
	double zCam = camera->z;

	// int dir = (int) ((2 * camera->rot) / PI + 4.5) % 4;

	int fov = HEIGHT;

	int y = 0;
	int x = 0;

	

	double yCenter = HEIGHT / 2;

	if (game->drawfloor) {
		for (y = 0; y < HEIGHT; y++) {
			double yd = ((y + 0.5) - yCenter) / fov;
			double zd = (4 + zCam * 8) / yd;
			if (yd < 0) {
				zd = (4 - zCam * 8) / -yd;
			}

			

			for (x = 0; x < WIDTH; x++) {
				double xd = (x - WIDTH / 2.0) / HEIGHT;
				xd *= zd;

				double xx = xd * rCos + zd * rSin + (xCam + 0.5) * 8;
				double yy = zd * rCos - xd * rSin + (yCam + 0.5) * 8;
				int xPix = (int) (xx * 2);
				int yPix = (int) (yy * 2);
				if (xx < 0) xPix--;
				if (yy < 0) yPix--;

				int texRow = 16;
				int texCol = 0;

				int tilex = xx / 8;
				int tiley = yy / 8;

				zBuffer[x + y * WIDTH] = zd;

				unsigned tile = getblock(level, xx / 8, yy / 8);
				unsigned color = tile;
				
				color = gpixel(textures, texRow + (xPix & 15), texCol + (yPix & 15));

				if (tilex < 0 || tilex > level.width + 1) continue;
				if (tiley < 0 || tiley > level.height + 1) continue;

				// if (y < yCenter) color = 0;
				display.pixels[x + y * WIDTH] = color;
			}
		}
	}

	#ifdef __RENDER_WALLS__
	if (game->drawwalls) {
		int xb = 0;
		int yb = 0;
		int xCenter = (int)floor(xCam);
		int zCenter = -(int)floor(yCam);
		for (xb = xCenter - game->renderdist; xb <= xCenter + game->renderdist; xb++) {
			for (yb = zCenter - game->renderdist; yb <= zCenter + game->renderdist; yb++) {
				uint32_t c = getblock(level, xb, -yb);
				if (c == 0xffffff) {
					// WEST
					if (getblock(level, xb - 1, -yb) == 0) {
						renderwall(&display, zBuffer, zBufferWall, camera, xb, yb, xb, yb + 1, c);
					}
					// EAST
					if (getblock(level, xb + 1, -yb) == 0) {
						renderwall(&display, zBuffer, zBufferWall, camera, xb + 1, yb + 1, xb + 1, yb, c);
					}
					// NORTH
					if (getblock(level, xb, -yb + 1) == 0) {
						renderwall(&display, zBuffer, zBufferWall, camera, xb + 1, yb, xb, yb, c);
					}
					// SOUTH
					if (getblock(level, xb, -yb - 1) == 0) {
						renderwall(&display, zBuffer, zBufferWall, camera, xb, yb + 1, xb + 1, yb + 1, c);
					}
				}
			}
		}
	}
	#endif

	// renderSprite(display, game, zBuffer, 4, 4, 0, 16, 16);	

	#ifdef __RENDER_POSTPROCESS__
	if (game->drawfog) {
		int fogdist = (int) (game->fogdist * 7);
		if (fogdist < 1) fogdist = 1;
		int ditherfactor = 4;
		for (int i = 0; i < WIDTH * HEIGHT; i++) {
			int xp = (i % WIDTH);
			int yp = (i / WIDTH) * 14;
			double zl = zBuffer[i];
			double xx = ((i % WIDTH - WIDTH / 2.0) / WIDTH);
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
	}
	#endif

	

	

	// int tcol = 48;
	// int trow = 0;
	// int texsize = 32;
	// int s = 3;
	// double scalesqrt = sqrt(s);
	// int xx = (int) (-camera->turnBob * 42) * scalesqrt;
	// int yy = (int) (sin(camera->bobPhase * 0.4) * 1 * camera->bob + camera->bob * 1) * scalesqrt * (camera->speed / camera->walkSpeed);
	// xx += WIDTH / 1.5;
	// // xx += sin(game->frames / 400.0) * 20;
	// yy += HEIGHT - texsize * s;
	// // yy += abs(sin(game->frames / 40.0) * 4);
	// yy += abs((int) ((-camera->turnBob * 20) * scalesqrt));
	// xx += (int) (sin(camera->bobPhase * 0.2) * 1 * camera->bob + camera->bob * 2) * scalesqrt;
	// for (x = 0; x < texsize * s; x++) {
	// 	int xPix = x + xx;
	// 	for (y = 0; y < texsize * s; y++) {
	// 		int yPix = y + yy;
	// 		unsigned color = gpixel(textures, y/s + tcol, x/s + trow);
	// 		if (color == 0xff00ff) continue;
	// 		setpixel(display, xPix, yPix, color);
	// 	}
	// }

	// #ifdef __RENDER_DEBUG_INFO__
	// gprintf(display, 3,  8, "Dir: %s", directions[dir]);
	// gprintf(display, 3, 16, "       x   y");
	// gprintf(display, 3, 26, "Tile: %2d, %2d", (int)floor(camera->x), (int)floor(camera->y));
	// gprintf(display, 3, 36, "frames: %d", game->frames);
	// #endif /* __RENDER_DEBUG_INFO__ */

	// for (int i = 0; i < WIDTH * HEIGHT; i++) display.pixels[i] = getcolor(100 - zBuffer[i], 100 - zBuffer[i], 100 - zBuffer[i]);
	if (game->drawprofiler == true) {
		
		int h;
		int profilercenter = HEIGHT / 4;

		gprintf(display, PROFILER_LOG_DEPTH + 3, profilercenter - FONT_CHAR_H - 2, "Logic");
		gprintf(display, PROFILER_LOG_DEPTH + 3, profilercenter + FONT_CHAR_H - 5, "Render");

		gprintf(display, 4, WIDTH/2, "(%f, %f)", game->camera->x, game->camera->y);

		
		for (int x = 0; x < PROFILER_LOG_DEPTH; x++) {
			h = (int) (ticktiminghistory[x] * 50000);
			for (int y = 0; y < h; y++) {
				int xx = PROFILER_LOG_DEPTH - x;
				int yy = profilercenter - y - 1;
				int color = lerpcolor(0x00ff00, getpixel(display, xx, yy), 0.5);
				setpixel(display, xx, yy, color);
			}
			h = (int) (frametiminghistory[x] * 2000);
			for (int y = 0; y < h; y++) {
				int xx = PROFILER_LOG_DEPTH - x;
				int yy = y;
				yy += profilercenter;
				int color = lerpcolor(0xD59F61, getpixel(display, xx, yy), 0.5);
				setpixel(display, xx, yy, color);
			}
		}
	}

	scriptdraw(game);


	unlock(context);
	present(context);

	profilerendframe();
}






/**
 * renderwall
 * 
 * This function takes a display, zBuffers, a camera reference, and locations
 * The order of locations are important as they determine the side of the wall to draw textures to
 */
#ifdef __RENDER_WALLS__
static inline void
renderwall (display_t *d, unsigned* zbuffer, int* zBufferWall, camera_t *cam, double x0, double y0, double x1, double y1, uint32_t c) {


	/**
	 * 
	 * Walls in this engine are rendered by giving the
	 * left position and the right position in and x,y
	 * plane.
	 * 
	 * You can visualize this with this diagram.
	 * 
	 *       +z    +-------------+
	 *             |             |
	 *             |             |
	 *             |     WALL    |
	 *             |             |
	 *       -z    +-------------+
	 *             ^             ^
	 *             (x0, y0)      (x1, y1)
	 * 
	 * It will only ever draw the wall if the left side is the
	 * coordinate for (x0, y0) as it draws left to right
	 */
	

	/**
	 * Here we get quick access to the position of the camera,
	 * The accessing and order is strange because in graphics
	 * Z is not up and down, 
	 */
	double xCam = cam->x - 0.5;
	double yCam = -cam->y + 0.5;
	double zCam = -cam->z;
	double rCos = -cos(cam->rot);
	double rSin = sin(cam->rot);


	// Get the center of the screen for future use
	double xCenter = WIDTH / 2;
	double yCenter = HEIGHT / 2;
	
	/**
	 * Throught the engine, the fov is the height of the window
	 */
	int fov = HEIGHT;

	/**
	 * Offset one end of the wall to the correct location
	 * Without this, the left part of the wall would be drawn
	 * so that it appears to pass through the camera's location
	 * but start off to the left.
	 */
	double xc0 = ((x0 - 0.5) - xCam) * 2;
	double yc0 = ((y0 - 0.5) - yCam) * 2;

	/**
	 * Shift that edge to somewhere along the center of
	 * the screen based on the camera's angle relative to the
	 * edge of the wall
	 * zz0 is the z distance (how far away from the camera)
	 * the corner of the wall is.
	 * 
	 * zz0 and zz1 can be visualized like this.
	 *             +-------------+
	 *             |             |
	 *             |             |
	 *             |    WALL     |
	 *             |             |
	 *             +-------------+
	 *             ^             ^
	 *             (x0, y0)      (x1, y1)
	 *        +-   \             /   -+
	 *        |     \           /     |
	 *        |      \         /      |
	 *  zz0 > |       \       /       | < zz1
	 *        |        \     /        |
	 *        |         \   /         |
	 *        +-        Camera       -+
	 * 
	 * 
	 */
	double xx0 = xc0 * rCos - yc0 * rSin;
	double u0 = ((-0.5) - zCam) * 2;
	double l0 = ((+0.5) - zCam) * 2;
	double zz0 = yc0 * rCos + xc0 * rSin;

	/*
	 * And now we do the same thing, but with the other edge
	 * of the wall, we shift it over and position it in
	 * perspective space.
	 */
	double xc1 = ((x1 - 0.5) - xCam) * 2;
	double yc1 = ((y1 - 0.5) - yCam) * 2;

	/**
	 * Yet again, shift that right edge to some location along
	 * the screen so that we can lerp between the two
	 */
	double xx1 = xc1 * rCos - yc1 * rSin;
	double u1 = ((-0.5) - zCam) * 2;
	double l1 = ((+0.5) - zCam) * 2;
	double zz1 = yc1 * rCos + xc1 * rSin;


	// The texture of the wall
	double xt0 = 0;
	double xt1 = 1;

	xt0 *= 16;
	xt1 *= 16;

	int tex = 0;
	int col = 0;

	double zClip = 0.1;

	/**
	 * Dont render the wall if it is too far out of frame
	 */
	if (zz0 < zClip && zz1 < zClip) return;

	/**
	 * Do further clipping because if the camera's angle
	 * to the wall is too shallow, and the wall is out of
	 * view, it draws the texture spread out to infinity.
	 * Here we clip the left side:
	 */
	if (zz0 < zClip) {
		double p = (zClip - zz0) / (zz1 - zz0);
		zz0 = zz0 + (zz1 - zz0) * p;
		xx0 = xx0 + (xx1 - xx0) * p;
		xt0 = xt0 + (xt1 - xt0) * p;
	}
	/**
	 * Here we clip the right side
	 */
	if (zz1 < zClip) {
		double p = (zClip - zz0) / (zz1 - zz0);
		zz1 = zz0 + (zz1 - zz0) * p;
		xx1 = xx0 + (xx1 - xx0) * p;
		xt1 = xt0 + (xt1 - xt0) * p;
	}

	/**
	 * Here it is decided where the left and right
	 * side of the wall are located on the screen
	 *       +z    +-------------+
	 *             |             |
	 *             |             |
	 *             |             |
	 *             |             |
	 *       -z    +-------------+
	 *             ^             ^
	 *             xPixel0       xPixel1
	 * 
	 */
	double xPixel0 = xCenter - (xx0 / zz0 * fov);
	double xPixel1 = xCenter - (xx1 / zz1 * fov);

	/**
	 * If the locations overlap, or are reversed, stop rendering the wall
	 */
	if (xPixel0 >= xPixel1) return;
	/**
	 * convert the float values for the x locations into ints so we can
	 * use them to draw pixels to th e screen
	 */
	int xp0 = (int) ceil(xPixel0);
	int xp1 = (int) ceil(xPixel1);

	/**
	 * Make sure to bound the wall so it doesn't seg fault by rendering outside
	 * the screen
	 */
	if (xp0 < 0) xp0 = 0;
	if (xp1 > WIDTH) xp1 = WIDTH;

	/**
	 * This is getting the Y position on the screen
	 * of all 4 corners of the wall, from here on, we
	 * just need to rasterize the texture on the wall
	 */
	double yTopLeft    = (u0 / zz0 * fov + yCenter);
	double yBottomLeft = (l0 / zz0 * fov + yCenter);
	double yTopRight = (u1 / zz1 * fov + yCenter);
	double yBottomRight = (l1 / zz1 * fov + yCenter);

	/**
	 * Now that we have the x and y locations of every
	 * corner of the wall, we can draw it to the screen
	 */
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

		double yPixel0 = yTopLeft + (yTopRight - yTopLeft) * pr - 0.5;
		double yPixel1 = yBottomLeft + (yBottomRight - yBottomLeft) * pr;

		int yp0 = (int) ceil(yPixel0);
		int yp1 = (int) ceil(yPixel1);
		if (yp0 < 0) yp0 = 0;
		if (yp1 > HEIGHT) yp1 = HEIGHT;

		double ih = 1 / (yPixel1 - yPixel0);
		for (int y = yp0; y < yp1; y++) {
			double pry = (y - yPixel0) * ih;
			int yTex = (int) (16 * pry);
			int xt = yTex + (tex % 8) * 16;
			int yt = xTex + (col / 8) * 16;
			int color = gpixel(textures, xt, yt);
			d->pixels[x + y * WIDTH] = color;
			zbuffer[x + y * WIDTH] = 1 / iz * 4;
		}
	}
}
#endif /* __RENDER_WALLS__*/






void
renderconsole(game_t* game, display_t d) {
	int x = 0;
	int y = 0;
	int i = 0;
	int linec = game->console->linec;
	char** linev = game->console->linev;
	int lineheight = FONT_CHAR_H + 5;
	int linestorender = HEIGHT / lineheight - 1;
	for (x = 0; x < WIDTH; x++) {
		for (y = 0; y < HEIGHT; y++) {
			unsigned color = d.pixels[x + y * WIDTH];
			setpixel(d, x, y, colormul(color, 0.1));
		}
	}
	int currentline = 0;
	for (i = linec - linestorender; i < linec; i++) {
		if (i >= 0) {
			gprintf(d, 2, lineheight * currentline, "%s", linev[i]);
			currentline++;
		}
		
	}
	

	gprintf(d, 2, lineheight * currentline, ">> %s", game->console->input);
	int cursoroffset = strlen(game->console->input) + 3;
	
	if (sin(game->frames / 10.0) < 0) {
		gprintf(d, FONT_CHAR_W * cursoroffset, lineheight * currentline, "|");
	}
	// textcolor = 0xffffff;
	
	
}



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
					setpixel(d, x + xo, y + yo, textcolor);
					setpixel(d, x + xo, y + yo + 1, colormul(textcolor, 0.4));
					// setpixel(d, x + xo + 2, y + yo + 2, colormul(textcolor, 0.4));
				}
			}
		}
		
	}
}


void
setpixel(display_t d, int x, int y, unsigned color) {
	if (x < WIDTH && x >= 0 && y >= 0 && y < HEIGHT)
		d.pixels[x + y * WIDTH] = color;
}

unsigned
getpixel(display_t d, int x, int y) {
	if (x < WIDTH && x >= 0 && y >= 0 && y < HEIGHT)
		return d.pixels[x + y * WIDTH];
	return -1;
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




// LUA RENDER BINDINGS:
int l_setpixel(lua_State* L) {
	int x = (int)lua_tonumber(L, -3);
	int y = (int)lua_tonumber(L, -2);
	int c = (int)lua_tonumber(L, -1);
	setpixel(*globaldisplay, x, y, c);
	return 1;
}

int l_getpixel(lua_State* L) {
	int x = lua_tointeger(L, -3);
	int y = lua_tointeger(L, -2);
	lua_pushinteger(L, getpixel(*globaldisplay, x, y));
	return 1;
}





void
renderSprite(display_t d, game_t* game, unsigned* zBuffer, double x, double y, double z, int texx, int texy) {
	double xCam = game->camera->x;
	double zCam = -game->camera->y;
	double yCam = -game->camera->z;
	double rCos = -cos(game->camera->rot);
	double rSin = sin(game->camera->rot);


	double xc = (x - xCam) * 2 - rSin * 0.2;
	double yc = (-z - yCam) * 2;
	double zc = (-y - zCam) * 2 - rCos * 0.2;


	double xx = xc * rCos - zc * rSin;
	double yy = yc;
	double zz = zc * rCos + xc * rSin;
	
	

	if (zz < 0.1) return;
	int fov = HEIGHT;
	int xCenter = WIDTH / 2;
	int yCenter = HEIGHT / 2;

	double xPixel = xCenter - (xx / zz * fov);
	double yPixel = (yy / zz * fov + yCenter);

	double xPixel0 = xPixel - HEIGHT / zz;
	double xPixel1 = xPixel + HEIGHT / zz;

	double yPixel0 = yPixel - HEIGHT / zz;
	double yPixel1 = yPixel + HEIGHT / zz;

	int xp0 = (int) ceil(xPixel0);
	int xp1 = (int) ceil(xPixel1);
	int yp0 = (int) ceil(yPixel0);
	int yp1 = (int) ceil(yPixel1);

	if (xp0 < 0)
		xp0 = 0;
	if (xp1 > WIDTH)
		xp1 = WIDTH;
	if (yp0 < 0)
		yp0 = 0;
	if (yp1 > HEIGHT)
		yp1 = HEIGHT;
	zz *= 4;
	for (int yp = yp0; yp < yp1; yp++) {
		double ypr = (yp - yPixel0) / (yPixel1 - yPixel0);
		int yt = (int) (ypr * 16);
		
		for (int xp = xp0; xp < xp1; xp++) {
			double xpr = (xp - xPixel0) / (xPixel1 - xPixel0);
			int xt = (int) (xpr * 16);
			if (zBuffer[xp + yp * WIDTH] > zz) {
				int col = gpixel(textures, yt + texy, xt + texx);
				if (col != 0xff00ff) {
					d.pixels[xp + yp * WIDTH] = col;
					zBuffer[xp + yp * WIDTH] = zz;
				}
			}
		}
	}

}
