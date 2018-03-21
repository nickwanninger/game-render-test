#include <time.h>
#include <stdio.h>
#include "profiler.h"


clock_t framestart = 0;

void profilerstartframe() {
	framestart = clock();
}
void profilerendframe() {
	clock_t end = clock();
	double time_elapsed_in_seconds = (end - framestart) / (double) CLOCKS_PER_SEC;
	for (int i = PROFILER_LOG_DEPTH - 1; i > 0; i--) {
		frametiminghistory[i] = frametiminghistory[i - 1];
	}
	frametiminghistory[0] = time_elapsed_in_seconds;
}

clock_t tickstart = 0;
clock_t luastart = 0;
double currentluatime = 0;

void profilerstarttick() {
	tickstart = clock();
	
}

void profilerendtick() {
	clock_t end = clock();
	double time_elapsed_in_seconds = (end - tickstart) / (double) CLOCKS_PER_SEC;
	for (int i = PROFILER_LOG_DEPTH - 1; i > 0; i--) {
		ticktiminghistory[i] = ticktiminghistory[i - 1];
		luatiminghistory[i] = luatiminghistory[i - 1];
	}
	luatiminghistory[0] = time_elapsed_in_seconds;
	ticktiminghistory[0] = currentluatime;
	currentluatime = 0;	
}


void profilerstartlua() {
	luastart = clock();
}

void profilerendlua() {
	clock_t end = clock();
	double time_elapsed_in_seconds = (end - luastart) / (double) CLOCKS_PER_SEC;
	currentluatime += time_elapsed_in_seconds;
}