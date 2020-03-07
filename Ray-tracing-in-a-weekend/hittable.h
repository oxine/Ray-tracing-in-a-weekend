#pragma once
#include"ray.h"

class material;
class hittable;

struct hit_record {
	float t;
	vec3 p;
	vec3 normal;
	hittable *hitObject;
};

class hittable {
public:
	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec)const = 0;
	material* mat;
};



