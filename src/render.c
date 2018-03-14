#include "render.h"

extern long frames;
extern long ticks;


unsigned
getcolor(unsigned char r, unsigned char g, unsigned char b) {
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
render(context_t context, player_t* player, graphic_t texture) {
	// Get the time that this render started.
	int t0 = SDL_GetTicks();


	display_t display = lock(context);
	
	int height = context.h;
	int width = context.w;

	unsigned zBuffer[width * height];

	double rCos = cos(player->rot);
	double rSin = sin(player->rot);
	double xCam = player->pos.x;
	double yCam = player->pos.y;
	double zCam = 0;

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

			double xx = xd * rCos + zd * rSin + (xCam + 0.5);
			double yy = zd * rCos - xd * rSin + (yCam + 0.5);

			int xPix = (int) (xx * 2);
			int yPix = (int) (yy * 2);
			if (xx < 0) xPix--;
			if (yy < 0) yPix--;

			int texRow = 16;
			int texCol = 0;
			if (y > yCenter) texRow += 16;
			zBuffer[x + y * display.width] = zd;

			unsigned color = gpixel(&texture, texRow + (xPix & 15), texCol + (yPix & 15));
			display.pixels[x + y * width] = color;
		}
	}

	for (int i = 0; i < width * height; i++) {
		int xp = (i % width);
		int yp = (i / width) * 14;
		double zl = zBuffer[i];
		double xx = ((i % width - width / 2.0) / width);
		int col = display.pixels[i];
		int brightness = (int) (300 - zl * 6 * (xx * xx * 2 + 1));
		brightness = (brightness + ((xp + yp) & 3) * 4) >> 4 << 4;
		if (brightness < 0) brightness = 0;
		if (brightness > 255) brightness = 255;
		int r = (col >> 16) & 0xff;
		int g = (col >> 8) & 0xff;
		int b = (col) & 0xff;
		r = r * brightness / 255;
		g = g * brightness / 255;
		b = b * brightness / 255;
		display.pixels[i] = r << 16 | g << 8 | b;
	}

	// Increment the frame count
	frames++;
	// display_t the context
	unlock(context);
	present(context);
	// Get the time the render took
	int t1 = SDL_GetTicks();
	int ms = 15 - (t1 - t0);
	// // And cap the framerate
	SDL_Delay(ms < 0 ? 0 : ms);
}


void
setpixel(display_t d, int x, int y, unsigned color) {
	d.pixels[x + y * d.width] = color;
}



display_t lock(context_t context) {
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