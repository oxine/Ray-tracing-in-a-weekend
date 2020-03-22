#pragma once
#include "ray.h"
#include "hitable.h"
class material {
public:
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const = 0;
	vec3 albedo;
};

vec3 random_in_unit_sphere() {
	vec3 p;
	do {
		p = 2.0f*vec3(rand() / float(RAND_MAX + 1), rand() / float(RAND_MAX + 1), rand() / float(RAND_MAX + 1)) - vec3::one;
	} while (p.squared_length() > 1.0);
	return p;
}

class lambertian : public material {
public:
	lambertian(const vec3& a){
		albedo = a;
	}
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
		vec3 target = rec.p + rec.normal + random_in_unit_sphere();
		scattered = ray(rec.p, target - rec.p);
		attenuation = albedo;
		return true;
	}
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
		vec3 reflected = reflect(r_in.Direction(), rec.normal);
		scattered = ray(rec.p, Normalize(reflected) + fuzz*random_in_unit_sphere());
		attenuation = albedo;
		return (dot(scattered.Direction(),rec.normal)>0);// why < 90d?
	}
	float fuzz;
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
		vec3 reflected = reflect(r_in.Direction(), rec.normal);
		float ni_over_nt;
		attenuation = vec3(1.0f, 1.0f, 1.0f);
		vec3 refracted;
		float reflect_prob;
		float cosine;

		if (dot(r_in.Direction(), rec.normal) > 0) {
			outward_normal = -rec.normal;
			ni_over_nt = ref_idx;
			cosine = ref_idx * dot(r_in.Direction(), rec.normal) / r_in.Direction().length();
		}
		else {
			outward_normal = rec.normal;
			ni_over_nt = 1.0/ref_idx;
			cosine = - dot(r_in.Direction(), rec.normal) / r_in.Direction().length();
		}

		if (refract(r_in.Direction(), outward_normal, ni_over_nt, refracted)) {
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