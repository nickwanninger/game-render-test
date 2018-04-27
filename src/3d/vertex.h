#ifndef __VERTEX__
#define __VERTEX__


#include "vector3.h"

typedef struct {
	vector3_t position;
	vector3_t normal;
	vector3_t texture;
} vertex_t;



typedef struct {
	int count;
	vertex_t* vertices;
} face_t;



#endif