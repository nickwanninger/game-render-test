#ifndef __PROFILER__
#define __PROFILER__

#include <time.h>


#define PROFILER_LOG_DEPTH 150

// int logictiminghistory[PROFILER_LOG_DEPTH];

double frametiminghistory[PROFILER_LOG_DEPTH];
double ticktiminghistory[PROFILER_LOG_DEPTH];
double luatiminghistory[PROFILER_LOG_DEPTH];


void profilerstartframe();
void profilerendframe();
void profilerstarttick();
void profilerendtick();

void profilerstartlua();
void profilerendlua();

#endif