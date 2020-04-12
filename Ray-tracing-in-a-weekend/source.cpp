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


int main() {
	clock_t tstart,tend;
	tstart = clock();
	srand(tstart);//if you do not reset the seed, you'll get the all same output


	const int image_width = 200;
	const int image_height = 100;


	for (int j = image_height - 1; j >= 0; --j) {
		for (int i = 0; i < image_width; ++i) {
			auto r = double(i) / image_width;
			auto g = double(j) / image_height;
			auto b = 0.2;
			int ir = static_cast<int>(255.999 * r);
			int ig = static_cast<int>(255.999 * g);
			int ib = static_cast<int>(255.999 * b);
			
		}
	}

	tend = clock();
	double endtime = (double)(tend - tstart) / CLOCKS_PER_SEC;
	cout << "Total time:" << endtime << endl;
	stbi_write_jpg("..//output.jpg", image_width, image_height, 3, data, 100);
	delete[] data;
	system("pause");
	return 0;
}