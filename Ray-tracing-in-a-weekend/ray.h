#pragma once
#include "vec3.h"
class ray {
private:
	vec3 origin;
	vec3 dir;
public:
	ray() {}
	ray(const vec3& origin, const vec3& dir) { this->origin = origin; this->dir = dir; }
	vec3 Origin() const {
		return origin;
	}
	vec3 Direction()const {
		return dir;
	}
	vec3 Point_on_ray(const float t) const{
		return origin + dir * t;
	}
};