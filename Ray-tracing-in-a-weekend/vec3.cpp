#include"vec3.h"
vec3 vec3::one = vec3(1, 1, 1);
vec3 vec3::zero = vec3(0, 0, 0);
vec3 vec3::up = vec3(0, 1, 0);

float random_float() {
	return rand() / (float)(RAND_MAX + 1);
}