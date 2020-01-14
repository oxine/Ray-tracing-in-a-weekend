#define STB_IMAGE_WRITE_IMPLEMENTATION
#include<iostream>
#include"ray.h"
#include"stb_image_write.h"
using namespace std;

vec3 color(const ray& r) {
	vec3 unit_direction = Normalize(r.Direction());
	float t = 0.5f*(unit_direction.y() + 1.0f);
	return  (1.0f-t)*vec3::one+t*vec3(0.5f,0.7f,1.0f);
}

int main() {
	vec3 lower_left_corner(-2.0f, -1.0f, -1.0f);
	vec3 horizontal(4.0f, 0.0f, 0.0f);
	vec3 vertical(0.0f, 2.0f, 0.0f);
	vec3 origin = vec3::zero;
	int x = 200;
	int y = 100;
	unsigned char* data = new unsigned char[x*y * 3];
	for(int j = 0;j < y;j++)
		for (int i = 0; i < x; i++)
		{
			float u = i / (float)x;
			float v = j / (float)y;
			ray r(origin, lower_left_corner + u * horizontal + v * vertical);
			vec3 col = color(r);
			data[j*x * 3 + i * 3] = unsigned char(255.99f * col[0]);
			data[j*x * 3 + i * 3 + 1] = unsigned char(255.99f * col[1]);
			data[j*x * 3 + i * 3 + 2] = unsigned char(255.99f * col[2]);
		}
	stbi_write_jpg("output.jpg", x, y, 3, data, 100);
	delete[] data;
	system("pause");
	return 0;
}