#define STB_IMAGE_WRITE_IMPLEMENTATION
#include<iostream>
#include <stdlib.h>
#include "stb_image_write.h"
#include "sphere.h"
#include "hitable_list.h"
#include "camera.h"
#include "material.h"
using namespace std;



vec3 color(const ray& r,hitable *world,int depth) {
	hit_record rec;
	if (world->hit(r, 0.001f, FLT_MAX, rec)) {
		ray scattered;
		vec3 attenuation;
		if (depth < 50 && rec.hitObject->mat->scatter(r, rec, attenuation, scattered)) {
			return attenuation*color(scattered, world,depth + 1);
		}
		else {
			return  vec3(1,0,0);
		}	
	}
	else {
		vec3 unit_direction = Normalize(r.Direction());
		float t = 0.5f*(unit_direction.y() + 1.0f);//a map as is mentioned above
		return  (1.0f - t)*vec3::one + t*vec3(0.5f, 0.7f, 1.0f);//a simple liner lerp from white(1,1,1) to azure(0.5f,0.7f,1.0f)	
	}
}

int main() {
	//srand(0);if you do not reset the seed, you'll get the all same output
	int x = 200;
	int y = 100;
	int s = 100;
	float gamma = 1.8f;
	hitable *list[4];
	list[0] = new sphere(vec3(0, 0, -1), 0.5f, new lambertian(vec3(0.8f, 0.3f, 0.3f)));
	list[1] = new sphere(vec3(0, -100.5f, -1), 100, new lambertian(vec3(0.8f, 0.8f, 0.0f)));
	list[2] = new sphere(vec3(1, 0, -1), 0.5f, new metal(vec3(0.8f, 0.6f, 0.2f)));
	list[3] = new sphere(vec3(-1, 0, -1), 0.5f, new metal(vec3(0.8f, 0.8f, 0.8f)));
	hitable *world = new hitable_list(list, 4);
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
				col += color(r, world,0);//make each point a color, depth:reflect count
			}
			col /= float(s);
			data[(y - j - 1)*x * 3 + i * 3] = unsigned char(255.99f * pow(col[0],1/ gamma));
			data[(y - j - 1)*x * 3 + i * 3 + 1] = unsigned char(255.99f * pow(col[1], 1 / gamma));
			data[(y - j - 1)*x * 3 + i * 3 + 2] = unsigned char(255.99f * pow(col[2], 1 / gamma));
		}
	stbi_write_jpg("..//output.jpg", x, y, 3, data, 100);
	delete[] data;
	//system("pause");
	return 0;
}