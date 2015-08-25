#include <graphics.h>
#include <conio.h>
#include <assert.h>
#include <cstdio>
#include <stdlib.h>
#include "matrix.h"
#include "ifs.h"

// 主函数
void main(int argc, char* argv[])
{
	// ========================================================
	// ========================================================
	// Some sample code you might like to use for parsing 
	// command line arguments and the input IFS files

	// sample command line:
	// ifs -input fern.txt -points 10000 -iters 10 -size 100 -output fern.tga

	char *input_file = NULL;// 获取 IFS 名称
	int num_points = 10000;
	int num_iters = 10;// 获取迭代次数
	int size = 100;
	char *output_file = NULL;

	for (int i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-input")) {
			i++; assert(i < argc);
			input_file = argv[i];
		}
		else if (!strcmp(argv[i], "-points")) {
			i++; assert(i < argc);
			num_points = atoi(argv[i]);
		}
		else if (!strcmp(argv[i], "-iters")) {
			i++; assert(i < argc);
			num_iters = atoi(argv[i]);
		}
		else if (!strcmp(argv[i], "-size")) {
			i++; assert(i < argc);
			size = atoi(argv[i]);
		}
		else if (!strcmp(argv[i], "-output")) {
			i++; assert(i < argc);
			output_file = argv[i];
		}
		else {
			printf("whoops error with command line argument %d: '%s'\n", i, argv[i]);
			assert(0);
		}
	}

	// ========================================================
	// ========================================================
	// Some sample code you might like to use for
	// parsing the IFS transformation files

	// open the file
	FILE *input = fopen(input_file, "r");
	assert(input != NULL);

	// read the number of transforms
	int num_transforms;// 压缩仿射变换族数
	fscanf(input, "%d", &num_transforms);

	// < DO SOMETHING WITH num_transforms >
	IFS *ifs = new IFS();
	//ifs->Set(num_transforms);

	// read in the transforms
	Matrix matrices[10];
	float probability[10];
	float now = 0;
	for (int i = 0; i < num_transforms; i++) {
		fscanf(input, "%f", &probability[i]);// 各压缩仿射变换对应的伴随概率
		probability[i] += now;
		now = probability[i];
		Matrix m;
		m.Read3x3(input);//读取仿射变换矩阵,读的是0、1、3行
		// < DO SOMETHING WITH probability and m >
		matrices[i] = m;
		//ifs->Set(i,m,probability);
	}

	// close the file
	fclose(input);

	/*
	Write the main program main.C that 
	1.creates an Image instance,
	2.reads an IFS description from a file, 
	3.renders the IFS to the image, 
	4.and saves the image.
	*/
	Image *image = new Image(size,size);

	ifs->input(num_transforms, matrices, probability);

	printf("%d\n", num_transforms);
	for (int k = 0; k < num_transforms; k++){
		printf("%d :\n", k);
		printf("%.3f\n", probability[k]);
		for (int i = 0; i < 4; i++){
			for (int j = 0; j < 4; j++)
			{
				printf("%.1f ", matrices[k].Get(j, i));
			}
			printf("\n");
		}
	}

	ifs->work(image,num_points,num_iters,size,output_file);

	image->SaveTGA(output_file);

	//getchar();
	// ========================================================
	// ========================================================
}