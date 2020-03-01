#pragma once
#include "hitable.h"

class sphere :public hitable {
public:
	sphere() {}
	sphere(vec3 c, float r,material* m) :center(c), radius(r){
		mat = m;
	};
	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec)const;
	vec3 center;
	float radius;
	
};


/*
line-sphere intersection:https://en.wikipedia.org/wiki/Line-sphere_intersection
*/
bool sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec)const {
	vec3 oc = r.Origin() - center;
	float a = dot(r.Direction(), r.Direction());
	float b = dot(r.Direction(), oc);// add *2
	float c = dot(oc, oc) - radius*radius;
	float discriminate = b * b - a * c;// remove *4 
	if (discriminate > 0) {
		float temp = (-b - sqrt(b*b - a*c)) / a;// remove *2
		if (temp<t_max&&temp>t_min) {
			rec.t = temp;
			rec.p = r.Point_on_ray(rec.t);
			rec.normal = (rec.p - center) / radius;
			rec.hitObject = (hitable*)this;
			return true;
		}
		temp = (-b + sqrt(b*b - a*c)) / a;// there's a range, so we should check both of them
		if (temp<t_max&&temp>t_min) {
			rec.t = temp;
			rec.p = r.Point_on_ray(rec.t);
			rec.normal = (rec.p - center) / radius;
			rec.hitObject = (hitable*)this;
			return true;
		}
	}
	return false;
}