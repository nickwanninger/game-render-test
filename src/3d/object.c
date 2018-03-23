#include "object.h"
#include <stdio.h>
#include <stdlib.h>

obj3d_t obj3dopen(char* filename) {
	obj3d_t object = {};
	object.filename = filename;
	FILE* f;
	f = fopen(filename, "r");
	if (f == NULL) {
		fprintf(stderr, "[object.c] Error loading object file '%s'\n", filename);
	}
}