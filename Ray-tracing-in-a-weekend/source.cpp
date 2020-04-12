#define STB_IMAGE_WRITE_IMPLEMENTATION
#include<iostream>
#include<iomanip>
#include <cstdlib>
#include <omp.h>
#include<ctime>
#include <memory>
#include "stb_image_write.h"
#include "rtweekend.h"
#include "material.h"
#include "camera.h"
#include "hittable_list.h"
#include "sphere.h"



using namespace std;

vec3 ray_color(const ray& r, const hittable& world, int depth) {
	hit_record rec;

	// If we've exceeded the ray bounce limit, no more light is gathered.
	if (depth <= 0)
		return vec3(0, 0, 0);

	if (world.hit(r, 0.001, infinity, rec)) {
		ray scattered;
		vec3 attenuation;
		if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
			return attenuation * ray_color(scattered, world, depth - 1);
		return vec3(0, 0, 0);
	}

	vec3 unit_direction = unit_vector(r.direction());
	auto t = 0.5*(unit_direction.y() + 1.0);
	return (1.0 - t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
}

hittable_list random_scene() {
	hittable_list world;

	world.add(make_shared<sphere>(
		vec3(0, -1000, 0), 1000, make_shared<lambertian>(vec3(0.5, 0.5, 0.5))));

	int i = 1;
	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			auto choose_mat = random_double();
			vec3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());
			if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
				if (choose_mat < 0.8) {
					// diffuse
					auto albedo = vec3::random() * vec3::random();
					world.add(
						make_shared<sphere>(center, 0.2, make_shared<lambertian>(albedo)));
				}
				else if (choose_mat < 0.95) {
					// metal
					auto albedo = vec3::random(.5, 1);
					auto fuzz = random_double(0, .5);
					world.add(
						make_shared<sphere>(center, 0.2, make_shared<metal>(albedo, fuzz)));
				}
				else {
					// glass
					world.add(make_shared<sphere>(center, 0.2, make_shared<dielectric>(1.5)));
				}
			}
		}
	}

	world.add(make_shared<sphere>(vec3(0, 1, 0), 1.0, make_shared<dielectric>(1.5)));

	world.add(
		make_shared<sphere>(vec3(-4, 1, 0), 1.0, make_shared<lambertian>(vec3(0.4, 0.2, 0.1))));

	world.add(
		make_shared<sphere>(vec3(4, 1, 0), 1.0, make_shared<metal>(vec3(0.7, 0.6, 0.5), 0.0)));

	return world;
}

int main() {
	clock_t tstart,tend;
	tstart = clock();
	srand(tstart);//if you do not reset the seed, you'll get the all same output

	
	const int image_width = 200;
	const int image_height = 100;
	const int samples_per_pixel = 100;
	const int channel = 3;
	const int max_depth = 50;
	auto data = new unsigned char[image_width*image_height*channel];

	vec3 lower_left_corner(-2.0, -1.0, -1.0);
	vec3 horizontal(4.0, 0.0, 0.0);
	vec3 vertical(0.0, 2.0, 0.0);
	vec3 origin(0.0, 0.0, 0.0);

	auto world = random_scene();

	vec3 lookfrom(13, 2, 3);
	vec3 lookat(0, 0, 0);
	vec3 vup(0, 1, 0);
	const auto aspect_ratio = double(image_width) / image_height;
	auto dist_to_focus = 10.0;
	auto aperture = 0.1;
	camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);
	for (int j = image_height - 1; j >= 0; --j) {
		std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
		for (int i = 0; i < image_width; ++i) {
			vec3 color(0, 0, 0);
			for (int s = 0; s < samples_per_pixel; ++s) {
				auto u = (i + random_double()) / image_width;
				auto v = (j + random_double()) / image_height;
				ray r = cam.get_ray(u, v);
				color += ray_color(r, world, max_depth);
			}
			int pos = (image_height - j - 1)*image_width * 3 + i * 3;
			color.write_color(data, pos, samples_per_pixel);
			cout << setprecision(2) << 100 * (image_width*j + i + 1) / (float)(image_height*image_width) << "%" << endl;
		}
	}

	tend = clock();
	double endtime = (double)(tend - tstart) / CLOCKS_PER_SEC;
	cout << "Done in" << endtime << endl;
	stbi_write_jpg("..//output.jpg", image_width, image_height, 3, data, 100);
	delete[] data;
	system("pause");
	return 0;
}