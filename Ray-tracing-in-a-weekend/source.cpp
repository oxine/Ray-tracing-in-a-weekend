#define STB_IMAGE_WRITE_IMPLEMENTATION
#include<iostream>
#include <stdlib.h>
#include "stb_image_write.h"
#include "sphere.h"
#include "hitable_list.h"
#include "camera.h"
using namespace std;

vec3 random_in_unit_sphere() {
	vec3 p;
	do {
		p = vec3(rand() / float(RAND_MAX + 1), rand() / float(RAND_MAX + 1),rand() / float(RAND_MAX + 1));
	} while (p.squared_length() > 1.0);
	return p;
}

vec3 color(const ray& r,hitable *world) {
	hit_record rec;
	if (world->hit(r, 0, FLT_MAX, rec)) {
		vec3 target = rec.p + rec.normal + random_in_unit_sphere();
		return 0.5f*color(ray(rec.p,target-rec.p),world);
	}
	else {
		vec3 unit_direction = Normalize(r.Direction());
		float t = 0.5f*(unit_direction.y() + 1.0f);//a map as is mentioned above
		return  (1.0f - t)*vec3::one + t*vec3(0.5f, 0.7f, 1.0f);//a simple liner lerp from white(1,1,1) to azure(0.5f,0.7f,1.0f)	
	}
}

int main() {
	srand(123);
	int x = 200;
	int y = 100;
	int s = 100;
	hitable *list[2];
	list[0] = new sphere(vec3(0, 0, -1), 0.5f);
	list[1] = new sphere(vec3(0, -100.5f, -1), 100);
	hitable *world = new hitable_list(list, 2);
	unsigned char* data = new unsigned char[x*y * 3];
	camera cam;
	for (int j = 0; j <y; j++)
		for (int i = 0; i < x; i++)
		{
			vec3 col = vec3::zero;
			for (int k = 0; k < s; k++) {
				float u = (i + rand() / float(RAND_MAX+1)) / (float)x;
				float v = (j + rand() / float(RAND_MAX+1)) / (float)y;
				ray r = cam.get_ray(u, v);
				//vec3 p = r.Point_on_ray(2.0f);
				col += color(r, world);//make each point a color
			}
			col /= float(s);
			data[(y - j - 1)*x * 3 + i * 3] = unsigned char(255.99f * col[0]);
			data[(y - j - 1)*x * 3 + i * 3 + 1] = unsigned char(255.99f * col[1]);
			data[(y - j - 1)*x * 3 + i * 3 + 2] = unsigned char(255.99f * col[2]);
		}
	stbi_write_jpg("..//output.jpg", x, y, 3, data, 100);
	delete[] data;
	//system("pause");
	return 0;
}