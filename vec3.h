#include<math.h>
#include<stdlib.h>
#include<iostream>

class vec3 {
	vec3() {};
	vec3(float x, float y, float z) { e[0] = x; e[1] = y; e[2] = z; }
	inline float x() const { return e[0]; };
	

	float e[3];
};