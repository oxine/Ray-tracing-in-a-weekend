#pragma once
#include "ray.h"

vec3 random_in_unit_disk() {
	vec3 p;
	do {
		p = 2.0f * vec3(rand() / (float)(RAND_MAX + 1), rand() / (float)(RAND_MAX + 1), 0);
	} while (dot(p, p) >= 1.0f);
	return p;
}

class camera {
public:
	camera(vec3 pos, vec3 lookat, vec3 up, float vfov, float aspect) {
		lens_radius = 0;

		float theta = vfov * M_PI / 180;
		float half_height = tan(theta / 2);//view aspect
		float half_width = aspect* half_height;

		origin = pos;
		w = Normalize(pos - lookat);
		u = cross(up, w).normalize();
		v = cross(w, u);

		lower_left_corner = origin - half_width * u - half_height*v - w;
		horizontal = 2 * half_width * u;
		vertical = 2 * half_height * v;
	}

	camera(vec3 pos, vec3 lookat, vec3 up, float vfov, float aspect, float aperture, float depth) {
		lens_radius = aperture / 2;

		float theta = vfov * M_PI / 180;
		float half_height = tan(theta / 2);//view aspect
		float half_width = aspect* half_height;
		
		origin = pos;
		w = Normalize(pos - lookat);
		u = Normalize(cross(up, w));
		v = cross(w, u);

		lower_left_corner = origin - (half_width * u + half_height*v + w)*depth;
		horizontal = 2 * half_width * u*depth;
		vertical = 2 * half_height * v*depth;
	}

	ray get_ray(float s, float t) {
		vec3 rd = lens_radius*random_in_unit_disk();
		vec3 offset = u*rd.x() + v*rd.y();//fix rd to uv plane
		return ray(origin + offset, lower_left_corner + s*horizontal + t*vertical-origin-offset);//from origin to the point on screen
	}

	float lens_radius;
	vec3 u, v, w;
	vec3 origin;
	vec3 lower_left_corner;
	vec3 horizontal;
	vec3 vertical;
};