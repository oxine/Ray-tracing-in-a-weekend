#pragma once
#include "vec3.h"
class ray {
private:
	vec3 o;
	vec3 dir;
public:
	ray() {}
	ray(const vec3& origin, const vec3& dir) { this->o = origin; this->dir = dir; }
	vec3 origin() const {
		return o;
	}
	vec3 direction()const {
		return dir;
	}
	vec3 point_on_ray(const float t) const{
		return o + dir * t;
	}
};