#include <stdio.h>
#include <stdlib.h>
#include "util.h"

void*
xmalloc(unsigned size) {
	void* new_mem = (void*) malloc(size);
	if (new_mem == NULL) {
		fprintf(stderr, "Fatal: memory exhausted (xmalloc of %u bytes).\n", size);
		exit(-1);
	}
	return new_mem;
}



void*
xcalloc(unsigned count, unsigned size) {
	void* new_mem = (void*) calloc(count, size);
	if (new_mem == NULL) {
		fprintf(stderr, "Fatal: memory exhausted (xcalloc of %u bytes).\n", size);
		exit(-1);
	}
	return new_mem;
}