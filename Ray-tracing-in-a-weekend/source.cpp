#define STB_IMAGE_WRITE_IMPLEMENTATION
#include<iostream>
#include"ray.h"
#include"stb_image_write.h"
using namespace std;

/*
line-sphere intersection:https://en.wikipedia.org/wiki/Line-sphere_intersection
*/
float hit_sphere(const vec3& center, float radius, const ray& r) {
	vec3 oc = r.Origin() - center;
	float a = dot(r.Direction(), r.Direction());
	float b = 2.0f * dot(r.Direction(), oc);
	float c = dot(oc, oc) - radius*radius;
	float discriminate = b * b - 4 * a * c;
	if (discriminate < 0)
		return -1.0f;
	else
		return ( - b - sqrtf(discriminate)) /( 2.0f * a);//hit-point nearer to the screen
}

vec3 color(const ray& r) {
	float t = hit_sphere(vec3(0.0f, 0.0f, -1.0f), 0.5f, r);	
	if (t > 0) {//now you can't hit the sphere behind you
		vec3 N = Normalize(r.Point_on_ray(t) - vec3(0, 0, -1));//o + dl - c  it's a unit normal
		return 0.5f*vec3(N.x()+1, N.y()+1, N.z()+1);//a map which makes[-1, 1] to[0, 1]
	}
	vec3 unit_direction = Normalize(r.Direction());
	t = 0.5f*(unit_direction.y() + 1.0f);//a map as is mentioned above
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
	//system("pause");
	return 0;
}