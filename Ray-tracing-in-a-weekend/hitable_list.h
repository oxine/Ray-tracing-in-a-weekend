#pragma once
#include "hitable.h"
class hitable_list :public hitable {
public:
	hitable_list() {}
	hitable_list(hitable **l, int n) { list = l;; list_size = n; }
	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec)const;
	hitable **list;
	int list_size;
};

bool hitable_list::hit(const ray& r, float t_min, float t_max, hit_record& rec)const {
	hit_record temp_rec;
	bool hit_anything = false;
	float closest_so_for = t_max;
	for (int i = 0; i < list_size; i++) {// a ray only hits the closest hitable
		if (list[i]->hit(r, t_min, closest_so_for, temp_rec)) {
			hit_anything = true;
			closest_so_for = temp_rec.t;
			rec = temp_rec;
		}
	}
	return hit_anything;
}