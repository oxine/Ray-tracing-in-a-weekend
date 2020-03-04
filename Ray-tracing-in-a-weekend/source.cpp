#define STB_IMAGE_WRITE_IMPLEMENTATION
#include<iostream>
#include<iomanip>
#include <cstdlib>
#include <omp.h>
#include<ctime>
#include "stb_image_write.h"
#include "sphere.h"
#include "hitable_list.h"
#include "camera.h"
#include "material.h"

using namespace std;
clock_t tstart, tend;


vec3 color(const ray& r,hitable *world,int depth) {
	hit_record rec;
	if (world->hit(r, 0.001f, FLT_MAX, rec)) {
		ray scattered;
		vec3 attenuation;
		if (depth < 50 && rec.hitObject->mat->scatter(r, rec, attenuation, scattered)) {
			return attenuation*color(scattered, world,depth + 1);
		}
		else {
			//return  vec3(1,0,0);
			return  vec3::zero;
		}	
	}
	else {
		vec3 unit_direction = Normalize(r.Direction());
		float t = 0.5f*(unit_direction.y() + 1.0f);//a map as is mentioned above
		return  (1.0f - t)*vec3::one + t*vec3(0.5f, 0.7f, 1.0f);//a simple liner lerp from white(1,1,1) to azure(0.5f,0.7f,1.0f)	
	}
}
float random_float() {
	return rand() / (float)(RAND_MAX + 1);
}

float random_float(float min, float max) {
	return min + (max - min)* rand() / (float)(RAND_MAX + 1);
}

hitable *random_scene() {
	int n = 500;
	hitable **list = new hitable*[n + 1];
	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(vec3(0.5, 0.5, 0.5)));
	int i = 1;
	#pragma omp parallel for num_threads(4)
	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			float choose_mat = random_float();
			vec3 center( a + 0.9f*random_float(),0.2f,b+ 0.9f*random_float());
			if ((center - vec3(4, 0.2f, 0)).length() > 0.9f) {
				if (choose_mat < 0.5) {
					list[i++] = new sphere(center, 0.2, new lambertian(vec3(random_float()*random_float(), random_float()*random_float(), random_float()*random_float())));
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
	list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(vec3(0.4f,0.2f,0.1f)));
	list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7,0.6,0.5),0));
	return new hitable_list(list, i);
}

int main() {
	tstart = clock();
	srand(tstart);//if you do not reset the seed, you'll get the all same output
	int x = 800;
	int y = 400;
	int s = 100;
	float gamma = 2.0f;
	hitable *list[5];
	list[0] = new sphere(vec3(0, 0, -1), 0.5f, new lambertian(vec3(0.8f, 0.3f, 0.3f)));
	list[1] = new sphere(vec3(0, -100.5f, -1), 100, new lambertian(vec3(0.8f, 0.8f, 0.0f)));
	list[2] = new sphere(vec3(1, 0, -1), 0.5f, new metal(vec3(0.8f, 0.6f, 0.2f),0.3f));
	list[3] = new sphere(vec3(-1, 0, -1), 0.5f, new dielectric(1.5f));
	list[4] = new sphere(vec3(-1, 0, -1), -0.45f, new dielectric(1.5f));
	//hitable *world = new hitable_list(random_scene(), 5);
	hitable *world = random_scene();

	unsigned char* data = new unsigned char[x*y * 3];
	vec3 lookfrom(12, 1.5, 4);
	vec3 lookat(0, 1 , 0);
	float dist_to_focus = (lookfrom - lookat).length();
	camera cam(lookfrom,lookat,vec3::up,30,x/(float)y,0,dist_to_focus);

	#pragma omp parallel for
	for (int j = 0; j <y; j++)
		for (int i = 0; i < x; i++)
		{
			vec3 col = vec3::zero;
			for (int k = 0; k < s; k++) {
				float u = (i + rand() / float(RAND_MAX + 1)) / (float)x;
				float v = (j + rand() / float(RAND_MAX + 1)) / (float)y;
				ray r = cam.get_ray(u, v);
				//vec3 p = r.Point_on_ray(2.0f);
				col += color(r, world, 0);//make each point a color, depth:reflect count
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