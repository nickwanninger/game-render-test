#ifndef __3D_OBJECT__
#define __3D_OBJECT__

#include "../gl/gllib.h"
#include "vertex.h"

typedef struct {
	char* filename;
	int facec;
	face_t* facev;
} obj3d_t;



obj3d_t obj3dopen(char* filename);


#endif