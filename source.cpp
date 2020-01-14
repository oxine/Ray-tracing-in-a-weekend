#define STB_IMAGE_WRITE_IMPLEMENTATION
#include<iostream>
#include"vec3.h"
#include"stb_image_write.h"
using namespace std;

int main() {
	vec3 a(1, 2, 3);
	//std::cout << a.normalize() << std::endl;
	int x = 200;
	int y = 200;
	unsigned char* data = new unsigned char[x*y * 3];
	for(int j = 0;j < y;j++)
		for (int i = 0; i < x; i++)
		{
			data[j*x * 3 + i * 3] = 255*i/(float)x;
			data[j*x * 3 + i * 3 + 1] = 255*j/(float)y;
			data[j*x * 3 + i * 3 + 2] = 50;
		}
	stbi_write_jpg("output.jpg", x, y, 3, data, 100);
	delete[] data;
	system("pause");
	return 0;
}