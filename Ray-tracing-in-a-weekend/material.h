#pragma once
#include "ray.h"
#include "hittable.h"
#include "perlin.h"



class texture {
public:
	virtual vec3 value(float u, float v, const vec3& p) const = 0;
};

class constant_texture : public texture {
public:
	constant_texture() {}
	constant_texture(vec3 c) : color(c) {}
	virtual vec3 value(float u, float v, const vec3& p) const {
		return color;
	}
	vec3 color;
};

class checker_texture : public texture {
public:
	checker_texture() {}
	checker_texture(texture *t0, texture *t1) : even(t0), odd(t1) {}
	virtual vec3 value(float u, float v, const vec3& p) const {
		float sines = sin(10 * p.x()) * sin(10 * p.y()) * sin(10 * p.z());
		if (sines < 0)
			return odd->value(u, v, p);
		else
			return even->value(u, v, p);
	}
	texture *odd;
	texture *even;
};


class material {
public:
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const = 0;
};

vec3 random_in_unit_sphere() {
	vec3 p;
	do {
		p = vec3(rand() / float(RAND_MAX + 1), rand() / float(RAND_MAX + 1), rand() / float(RAND_MAX + 1));
	} while (p.squared_length() > 1.0);
	return p;
}

class lambertian : public material {
public:
	lambertian(texture* a):albedo(a){}
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
		vec3 target = rec.p + rec.normal + random_in_unit_sphere();
		scattered = ray(rec.p, target - rec.p,r_in.time());
		attenuation = albedo->value(rec.u,rec.v,rec.p);
		
		return true;
	}
	texture* albedo;
};



vec3 reflect(const vec3& v, const vec3& n) {
	return v - 2 * dot(v, n)*n;// the degree of v, n > 90
}


class metal :public material {
public:
	metal(const vec3& a,float f) {
		if (f > 1.0)
			fuzz = 1.0f;
		else if (f < 0)
			fuzz = 0;
		else
			fuzz = f;
		albedo = a;
	}
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered)const {
		vec3 reflected = reflect(r_in.direction(), rec.normal);
		scattered = ray(rec.p, Normalize(reflected) + fuzz*random_in_unit_sphere());
		attenuation = albedo;
		return (dot(scattered.direction(),rec.normal)>0);// why < 90d?
	}
	float fuzz;
	vec3 albedo;
};

bool refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted) {
	vec3 unit_v = v.normalize();
	float dt = dot(unit_v, n);//dt, or we say, cos theta
	float square_cos_theta1 = 1.0f - ni_over_nt*ni_over_nt*(1 - dt*dt);
	if (square_cos_theta1 > 0) {
		refracted = ni_over_nt*(unit_v - n*dt) - n*sqrt(square_cos_theta1);
		return true;
	}
	else
		return false;//
}

//Fresnel - Schlick approximation
float schlick(float cosine, float ref_idx) {
	float r0 = (1 - ref_idx) / (1 + ref_idx);
	r0 = r0*r0;
	return r0 + (1 - r0)*pow((1-cosine),5);
}

class dielectric : public material {
public:
	dielectric(float ri) :ref_idx(ri) {
		
	}
	float ref_idx;
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered)const {
		vec3 outward_normal;
		vec3 reflected = reflect(r_in.direction(), rec.normal);
		float ni_over_nt;
		attenuation = vec3(1.0f, 1.0f, 1.0f);
		vec3 refracted;
		float reflect_prob;
		float cosine;

		if (dot(r_in.direction(), rec.normal) > 0) {
			outward_normal = -rec.normal;
			ni_over_nt = ref_idx;
			cosine = ref_idx * dot(r_in.direction(), rec.normal) / r_in.direction().length();
		}
		else {
			outward_normal = rec.normal;
			ni_over_nt = 1.0/ref_idx;
			cosine = - dot(r_in.direction(), rec.normal) / r_in.direction().length();
		}

		if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted)) {
			reflect_prob = schlick(cosine, ref_idx);
		}
		else {
			reflect_prob = 1.0f;
		}
		

		if (reflect_prob > (rand()/(float)(RAND_MAX))) {
			scattered = ray(rec.p, reflected);
		}
		else {
			scattered = ray(rec.p, refracted);
		}
		return true;
	}

};

class noise_texture : public texture {
public:
	noise_texture(float s):scale(s) {}
	virtual vec3 value(float u, float v, const vec3& p) const {
		return vec3(1, 1, 1) * 0.5 * (1 + sin(scale*p.z() + 10 * noise.turb(p)));
	}
	perlin noise;
	float scale;
};


class image_texture : public texture {
public:
	image_texture() {}
	image_texture(unsigned char *pixels, int A, int B)
		: data(pixels), nx(A), ny(B) {}
	virtual vec3 value(float u, float v, const vec3& p) const;
	unsigned char *data;
	int nx, ny;
};

vec3 image_texture::value(float u, float v, const vec3& p) const {
	int i = (u)* nx;
	int j = (1 - v) * ny - 0.001;
	if (i < 0) i = 0;
	if (j < 0) j = 0;
	if (i > nx - 1) i = nx - 1;
	if (j > ny - 1) j = ny - 1;
	float r = int(data[3 * i + 3 * nx*j]) / 255.0;
	float g = int(data[3 * i + 3 * nx*j + 1]) / 255.0;
	float b = int(data[3 * i + 3 * nx*j + 2]) / 255.0;
	return vec3(r, g, b);
}