#include <SDL2/SDL.h>
#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include "profiler.h"


uint64_t framestart = 0;

void profilerstartframe() {
	framestart = SDL_GetPerformanceCounter();
}


void profilerendframe() {
	uint64_t end = SDL_GetPerformanceCounter();
	double time_elapsed_in_seconds = (double)((end - framestart) * 1000) / SDL_GetPerformanceFrequency();
	for (int i = PROFILER_LOG_DEPTH - 1; i > 0; i--) {
		frametiminghistory[i] = frametiminghistory[i - 1];
	}
	frametiminghistory[0] = time_elapsed_in_seconds;
}

uint64_t tickstart = 0;
uint64_t luastart = 0;
double currentluatime = 0;

void profilerstarttick() {
	tickstart = SDL_GetPerformanceCounter();
	
}

void profilerendtick() {
	uint64_t end = SDL_GetPerformanceCounter();
	double time_elapsed_in_seconds = (double)((end - tickstart) * 1000) / SDL_GetPerformanceFrequency();
	for (int i = PROFILER_LOG_DEPTH - 1; i > 0; i--) {
		ticktiminghistory[i] = ticktiminghistory[i - 1];
		luatiminghistory[i] = luatiminghistory[i - 1];
	}
	luatiminghistory[0] = time_elapsed_in_seconds;
	ticktiminghistory[0] = currentluatime;
	currentluatime = 0;
}


void profilerstartlua() {
	luastart = SDL_GetPerformanceCounter();
}

void profilerendlua() {
	uint64_t end = SDL_GetPerformanceCounter();
	double time_elapsed_in_seconds = (double)((end - luastart) * 1000) / SDL_GetPerformanceFrequency();
	currentluatime += time_elapsed_in_seconds;
}