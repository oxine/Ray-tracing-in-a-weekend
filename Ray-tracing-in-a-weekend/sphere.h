#pragma once
#include "hittable.h"

class sphere :public hittable {
public:
	sphere() {}
	sphere(vec3 c, float r,material* m) :center(c), radius(r),mat(m){};
	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec)const;
	virtual bool bounding_box(float t0, float t1, aabb& box) const;
	vec3 center;
	float radius;
	material* mat;
};

/*
line-sphere intersection:https://en.wikipedia.org/wiki/Line-sphere_intersection
*/
bool sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec)const {
	vec3 oc = r.origin() - center;
	float a = dot(r.direction(), r.direction());
	float b = dot(r.direction(), oc);// add *2
	float c = dot(oc, oc) - radius*radius;
	float discriminate = b * b - a * c;// remove *4 
	if (discriminate > 0) {
		float temp = (-b - sqrt(b*b - a*c)) / a;// remove *2
		if (temp<t_max&&temp>t_min) {
			rec.t = temp;
			rec.p = r.point_on_ray(rec.t);
			rec.normal = (rec.p - center) / radius;
			rec.mat = mat;
			return true;
		}
		temp = (-b + sqrt(b*b - a*c)) / a;// there's a range, so we should check both of them
		if (temp<t_max&&temp>t_min) {
			rec.t = temp;
			rec.p = r.point_on_ray(rec.t);
			rec.normal = (rec.p - center) / radius;
			rec.mat = mat;
			return true;
		}
	}
	return false;
}

bool sphere::bounding_box(float t0, float t1, aabb& box) const {
	box = aabb(center - vec3(radius, radius, radius),
		center + vec3(radius, radius, radius));
	return true;
}

class moving_sphere :public hittable {
public:
	moving_sphere() {}
	moving_sphere(vec3 c0, vec3 c1, float t0, float t1, float r, material* m) :center0(c0), center1(c1), time0(t0), time1(t1), radius(r), mat(m) {};
	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec)const;
	virtual bool bounding_box(float t0, float t1, aabb& box) const;
	vec3 center0;
	vec3 center1;
	float time0, time1;
	float radius;
	material* mat;

	vec3 center(float time) const {
		return center0 + ((time - time0) / (time1 - time0))*(center1 - center0);
	}
};


bool moving_sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec)const {

	vec3 oc = r.origin() - center(r.time());
	float a = dot(r.direction(), r.direction());
	float b = dot(r.direction(), oc);// add *2
	float c = dot(oc, oc) - radius*radius;
	float discriminate = b * b - a * c;// remove *4 
	if (discriminate > 0) {
		float temp = (-b - sqrt(b*b - a*c)) / a;// remove *2
		if (temp<t_max&&temp>t_min) {
			rec.t = temp;
			rec.p = r.point_on_ray(rec.t);
			rec.normal = (rec.p - center(r.time())) / radius;
			rec.mat = mat;
			return true;
		}
		temp = (-b + sqrt(b*b - a*c)) / a;// there's a range, so we should check both of them
		if (temp<t_max&&temp>t_min) {
			rec.t = temp;
			rec.p = r.point_on_ray(rec.t);
			rec.normal = (rec.p - center(r.time())) / radius;
			rec.mat = mat;
			return true;
		}
	}
	return false;
}

bool moving_sphere::bounding_box(float t0, float t1, aabb& box) const {
	aabb box0(center(t0) - vec3(radius, radius, radius),
		center(t0) + vec3(radius, radius, radius));
	aabb box1(center(t1) - vec3(radius, radius, radius),
		center(t1) + vec3(radius, radius, radius));
	box = surrounding_box(box0, box1);
	return true;
}