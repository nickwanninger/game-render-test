#ifndef __point_DEF__
#define __point_DEF__

typedef struct {
	float x;
	float y;
} point_t;

typedef struct {
	float x;
	float y;
	float z;
} point3d_t;


point_t point_mul_scalar(point_t a, float m);

#endif