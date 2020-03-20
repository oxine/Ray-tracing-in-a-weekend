#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"
#include<iostream>
#include<iomanip>
#include <cstdlib>
#include <omp.h>
#include<ctime>
#include "sphere.h"
#include "hittable_list.h"
#include "camera.h"
#include "material.h"
#include "bvh_node.h"
#include "aarect.h"
#include "box.h"
using namespace std;
clock_t tstart, tend;


vec3 clampColor(vec3& c) {
	if (c.e[0]> 1.0f)
		c.e[0] = 1;
	if (c.e[1] > 1.0f)
		c.e[1] = 1;
	if (c.e[2] > 1.0f)
		c.e[2] = 1;
	return c;
}

vec3 ray_color(const ray& r, const vec3& background, const hittable& world, int depth) {
	hit_record rec;

	// If we've exceeded the ray bounce limit, no more light is gathered.
	if (depth <= 0)
		return vec3(0, 0, 0);

	// If the ray hits nothing, return the background color.
	if (!world.hit(r, 0.001, FLT_MAX, rec))
		return background;

	ray scattered;
	vec3 attenuation;
	vec3 emitted = rec.mat->emitted(rec.u, rec.v, rec.p);
	if (!rec.mat->scatter(r, rec, attenuation, scattered))
		return clampColor(emitted) ;


	

	return clampColor(emitted + attenuation * ray_color(scattered, background, world, depth - 1));
}

vec3 color(const ray& r, hittable *world, int depth) {
	hit_record rec;
	if (world->hit(r, 0.001, FLT_MAX, rec)) {
		ray scattered;
		vec3 attenuation;
		vec3 emitted = rec.mat->emitted(rec.u, rec.v, rec.p);
		if (depth < 50 && rec.mat->scatter(r, rec, attenuation, scattered))
			return emitted + attenuation*color(scattered, world, depth + 1);
		else
			return emitted;
	}
	else
		return vec3(0, 0, 0);
}



hittable *random_scene() {
	int n = 50000;
	hittable **list = new hittable*[n + 1];
	texture *checker = new checker_texture(
		new constant_texture(vec3(0.2, 0.3, 0.1)),
		new constant_texture(vec3(0.9, 0.9, 0.9))
	);
	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(checker));
	int i = 1;
	#pragma omp parallel for num_threads(4)
	for (int a = -10; a < 10; a++) {
		for (int b = -10; b < 10; b++) {
			float choose_mat = random_float();
			vec3 center( a + 0.9f*random_float(),0.2f,b+ 0.9f*random_float());
			if ((center - vec3(4, 0.2f, 0)).length() > 0.9f) {
				if (choose_mat < 0.65f) {  // diffuse
					list[i++] = new moving_sphere(
						center,
						center + vec3(0, 0.5f*random_float(), 0),
						0.0, 1.0, 0.2,
						new lambertian(new constant_texture(
							vec3(random_float()*random_float(),
								random_float()*random_float(),
								random_float()*random_float()))
						)
					);
				}
				else if(choose_mat < 0.8){
					list[i++] = new sphere(center, 0.2, new metal(vec3(random_float(0.5f, 1.0f), random_float(0.5f, 1.0f), random_float(0.5f, 1.0f)),1.5f));
				}
				else {
					list[i++] = new sphere(center, 0.2, new dielectric(1.5f));
				}
			}
		}
	}
	
	list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5f));
	list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(new constant_texture(vec3(0.4f,0.2f,0.1f))));
	list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7,0.6,0.5),0));
	return new bvh_node(list, i, 0, 1);
	//return new hittable_list(list, i);
}

hittable *two_perlin_spheres() {
	texture *pertext = new noise_texture(3);
	hittable **list = new hittable*[2];
	int nx, ny, nn;
	unsigned char *tex_data = stbi_load("earthmap.jpg", &nx, &ny, &nn, 0);
	material *mat = new lambertian(new image_texture(tex_data, nx, ny));
	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(pertext));
	list[1] = new sphere(vec3(0, 2, 0), 2, mat);
	return new hittable_list(list, 2);
}

hittable *simple_light() {
	texture *pertext = new noise_texture(4);
	hittable **list = new hittable*[4];
	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(pertext));
	list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian(pertext));
	list[2] = new sphere(vec3(0, 7, 0), 2,
		new diffuse_light(new constant_texture(vec3(4, 4, 4))));
	list[3] = new xy_rect(3, 5, 1, 3, -2,
		new diffuse_light(new constant_texture(vec3(4, 4, 4))));
	return new hittable_list(list, 4);
}

hittable *cornell_box() {
	hittable **list = new hittable*[6];
	int i = 0;
	material *red = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
	material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
	material *green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
	material *light = new diffuse_light(new constant_texture(vec3(15, 15, 15)));

	list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
	list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
	list[i++] = new xz_rect(213, 343, 227, 332, 554, light);
	list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
	list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
	list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));

	list[i++] = new translate(
		new rotate_y(new box(vec3(0, 0, 0), vec3(165, 165, 165), white), -18),
		vec3(130, 0, 65)
	);
	list[i++] = new translate(
		new rotate_y(new box(vec3(0, 0, 0), vec3(165, 330, 165), white), 15),
		vec3(265, 0, 295)
	);
	return new hittable_list(list, i);
}

int main() {
	const vec3 background(0,0,0);
	tstart = clock();
	srand(tstart);//if you do not reset the seed, you'll get the all same output
	int x = 400;
	int y = 200;
	int s = 40;
	float gamma = 2.0f;
	hittable *list[5];
	list[0] = new sphere(vec3(0, 0, -1), 0.5f, new lambertian(new constant_texture(vec3(0.8f, 0.3f, 0.3f))));
	list[1] = new sphere(vec3(0, -100.5f, -1), 100, new lambertian(new constant_texture(vec3(0.8f, 0.8f, 0.0f))));
	list[2] = new sphere(vec3(1, 0, -1), 0.5f, new metal(vec3(0.8f, 0.6f, 0.2f),0.3f));
	list[3] = new sphere(vec3(-1, 0, -1), 0.5f, new dielectric(1.5f));
	list[4] = new sphere(vec3(-1, 0, -1), -0.45f, new dielectric(1.5f));

	hittable *world = cornell_box();
	
	unsigned char* data = new unsigned char[x*y * 3];
	vec3 lookfrom(278, 278, -800);
	vec3 lookat(278, 278, 0);
	float dist_to_focus = 10.0;
	float aperture = 0.0;
	float vfov = 40.0;

	camera cam(lookfrom, lookat, vec3(0, 1, 0), vfov, float(x) / float(y),
		aperture, dist_to_focus, 0.0, 1.0);

	#pragma omp parallel for
	for (int j = 0; j <y; j++)
		for (int i = 0; i < x; i++)
		{
			vec3 col = vec3::zero;
			for (int k = 0; k < s; k++) {
				float u = (i + rand() / float(RAND_MAX + 1)) / (float)x;
				float v = (j + rand() / float(RAND_MAX + 1)) / (float)y;
				ray r = cam.get_ray(u, v);
				//vec3 p = r.point_on_ray(2.0f);
				col += ray_color(r, vec3::zero,*world, 50);//make each point a color, depth:reflect count
			}
			col /= float(s);
			data[(y - j - 1)*x * 3 + i * 3] = unsigned char(255.99f * pow(col[0], 1 / gamma));
			data[(y - j - 1)*x * 3 + i * 3 + 1] = unsigned char(255.99f * pow(col[1], 1 / gamma));
			data[(y - j - 1)*x * 3 + i * 3 + 2] = unsigned char(255.99f * pow(col[2], 1 / gamma));
			cout << setprecision(2) << 100 * (x*j + i + 1) / (float)(y*x) << "%" << endl;
		}
	
	tend = clock();
	double endtime = (double)(tend - tstart) / CLOCKS_PER_SEC;
	cout << "Total time:" << endtime << endl;
	stbi_write_jpg("..//output.jpg", x, y, 3, data, 100);
	delete[] data;
	system("pause");
	return 0;
}