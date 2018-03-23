#ifndef __3D_OBJECT__
#define __3D_OBJECT__

#include "../gl/gllib.h"

typedef struct {
	char* filename;
} obj3d_t;



obj3d_t obj3dopen(char* filename);


#endif