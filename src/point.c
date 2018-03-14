#include "point.h"


point_t
point_mul_scalar(point_t a, float m) {
	point_t p = { a.x * m, a.y * m};
	return p;
}