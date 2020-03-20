#pragma once
#include "vec3.h"
#include<memory>
#include"flipnormal.h"
float random_float() {
	return rand() / (float)(RAND_MAX + 1);
}

float random_float(float min,float max) {
	return min + (max-min)*rand() / (float)(RAND_MAX + 1);
}



class ray {
private:
	vec3 o;
	vec3 dir;
	float _time;
public:
	ray() {}
	ray(const vec3& origin,const vec3& dir, float t = 0.0f) { this->o = origin; this->dir = dir; _time = t; }
	vec3 origin() const {
		return o;
	}
	vec3 direction()const {
		return dir;
	}
	vec3 point_on_ray(const float t) const{
		return o + dir * t;
	}

	float time()const {
		return _time;
	}
};

