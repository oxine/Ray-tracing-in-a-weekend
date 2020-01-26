#define STB_IMAGE_WRITE_IMPLEMENTATION
#include<iostream>
#include"ray.h"
#include"stb_image_write.h"
using namespace std;

/*
line-sphere intersection:https://en.wikipedia.org/wiki/Line-sphere_intersection
*/
bool hit_sphere(const vec3& center, float radius, const ray& r) {
	vec3 oc = r.Origin() - center;
	float a = dot(r.Direction(), r.Direction());
	float b = 2.0f * dot(r.Direction(), oc);
	float c = dot(oc, oc) - radius*radius;
	float delta = b * b - 4 * a * c;
	return (delta >= 0);
}

vec3 color(const ray& r) {
	if (hit_sphere(vec3(0.0f, 0.0f, -1.0f), 0.5f, r))
		return vec3(1, 0, 0);
	vec3 unit_direction = Normalize(r.Direction());
	float t = 0.5f*(unit_direction.y() + 1.0f);//a map which makes [-1,1] to[0,1]
	return  (1.0f-t)*vec3::one+t*vec3(0.5f,0.7f,1.0f);//a simple liner lerp from white(1,1,1) to azure(0.5f,0.7f,1.0f)
}

int main() {
	vec3 lower_left_corner(-2.0f, -1.0f, -1.0f);
	vec3 horizontal(4.0f, 0.0f, 0.0f);
	vec3 vertical(0.0f, 2.0f, 0.0f);
	vec3 origin = vec3::zero;
	int x = 200;
	int y = 100;
	unsigned char* data = new unsigned char[x*y * 3];
	for (int j = 0; j <y; j++)
		for (int i = 0; i < x; i++)
		{
			float u = i / (float)x;
			float v = j / (float)y;
			ray r(origin, lower_left_corner + u * horizontal + v * vertical);//from origin to the point on screen
			vec3 col = color(r);//make each point a color
			data[(y - j - 1)*x * 3 + i * 3] = unsigned char(255.99f * col[0]);
			data[(y - j - 1)*x * 3 + i * 3 + 1] = unsigned char(255.99f * col[1]);
			data[(y - j - 1)*x * 3 + i * 3 + 2] = unsigned char(255.99f * col[2]);
		}
	stbi_write_jpg("..//output.jpg", x, y, 3, data, 100);
	delete[] data;
	vec3 a(1, 2, 3);
	cout << dot(a, a) << endl;
	vec3 b(4, 5, 6);
	cout << a - b << endl;
	//system("pause");
	return 0;
}